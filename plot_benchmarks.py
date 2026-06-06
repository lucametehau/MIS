#!/usr/bin/env python3
"""Generate benchmark plots from config-harness CSV files."""

from __future__ import annotations

import argparse
from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd

# Default run directory (override with --results-dir)
DEFAULT_RESULTS = Path(__file__).resolve().parent
DEFAULT_OUT = Path(__file__).resolve().parent / "report" / "figures"

ALGO_COLORS = {
    "Sequential CSR": "#4C72B0",
    "Sequential": "#4C72B0",
    "Luby 16t CSR": "#DD8452",
    "Luby 16t": "#DD8452",
    "Luby Improved 16t CSR": "#55A868",
    "Luby GPU CSR": "#C44E52",
}
LUBY_COLOR = "#DD8452"
IMPROVED_COLOR = "#55A868"
GPU_COLOR = "#C44E52"


def load_csv(results_dir: Path, name: str) -> pd.DataFrame | None:
    path = results_dir / f"{name}.csv"
    if not path.exists():
        print(f"  skipping {name} (not found)")
        return None
    return pd.read_csv(path)


def save_fig(fig: plt.Figure, out_dir: Path, stem: str) -> None:
    out_dir.mkdir(parents=True, exist_ok=True)
    for ext in ("pdf", "png"):
        fig.savefig(out_dir / f"{stem}.{ext}", bbox_inches="tight", dpi=150)
    plt.close(fig)
    print(f"  wrote {stem}.pdf / .png")


def plot_section0(df: pd.DataFrame, out_dir: Path) -> None:
    fig, axes = plt.subplots(1, 2, figsize=(9, 4), sharey=True)
    for ax, label in zip(axes, ["uniform_1m_c10", "scale_free_1m"]):
        sub = df[df["label"] == label]
        x_labels = []
        means, stds, colors = [], [], []
        for _, row in sub.iterrows():
            short = row["algorithm"].replace(" CSR", "")
            rep = "CSR" if row["representation"] == "csr" else "Adj"
            x_labels.append(f"{short}\n({rep})")
            means.append(row["mean_time_ms"])
            stds.append(row["graph_std_ms"])
            colors.append("#55A868" if rep == "CSR" else "#C44E52")
        x = range(len(x_labels))
        ax.bar(x, means, yerr=stds, capsize=3, color=colors, edgecolor="white", linewidth=0.5)
        ax.set_xticks(x, x_labels, fontsize=8)
        ax.set_title(label.replace("_", " "))
        ax.set_ylabel("Time (ms)")
        ax.grid(axis="y", alpha=0.3)
    fig.suptitle("§0: CSR vs adjacency ($N=10^6$)", fontsize=11)
    fig.tight_layout()
    save_fig(fig, out_dir, "section0_csr_justify")


def plot_section1(df: pd.DataFrame, out_dir: Path) -> None:
    fig, axes = plt.subplots(1, 2, figsize=(10, 4.5))
    specs = [
        ("uniform_sparse", "uniform_", "Uniform sparse ($c=10$)"),
        ("scale_free", "scale_free_", "Scale-free ($m_0=10$, $m=5$)"),
    ]
    for ax, (family, prefix, title) in zip(axes, specs):
        sub = df[df["graph_family"] == family]
        for algo, style in [
            ("Sequential CSR", {"marker": "o", "ls": "-"}),
            ("Luby 16t CSR", {"marker": "s", "ls": "--"}),
            ("Luby Improved 16t CSR", {"marker": "^", "ls": "-."}),
            ("Luby GPU CSR", {"marker": "D", "ls": ":", "color": GPU_COLOR}),
        ]:
            rows = sub[sub["algorithm"] == algo].sort_values("n")
            if rows.empty:
                continue
            ax.errorbar(
                rows["n"],
                rows["mean_time_ms"],
                yerr=rows["graph_std_ms"],
                label=algo.replace(" CSR", ""),
                capsize=3,
                **style,
            )
        ax.set_xscale("log")
        ax.set_yscale("log")
        ax.set_xlabel("$N$")
        ax.set_ylabel("Time (ms)")
        ax.set_title(title)
        ax.legend(fontsize=8)
        ax.grid(True, which="both", alpha=0.3)
    fig.suptitle("§1: Scaling with $N$ (CSR, 16 threads)", fontsize=11)
    fig.tight_layout()
    save_fig(fig, out_dir, "section1_scaling_n")


def _thread_scaling_rows(sub: pd.DataFrame, variant: str) -> pd.DataFrame:
    """Select Luby vs Luby Improved rows (avoid matching both on startswith('Luby'))."""
    if variant == "luby":
        mask = sub["algorithm"].str.match(r"^Luby \d+t ", na=False)
    else:
        mask = sub["algorithm"].str.match(r"^Luby Improved \d+t ", na=False)
    return sub[mask].sort_values("threads")


def plot_section2(df: pd.DataFrame, out_dir: Path) -> None:
    fig, axes = plt.subplots(1, 2, figsize=(10, 4.5))
    for ax, label, title in zip(
        axes,
        ["uniform_5m_c10", "scale_free_5m"],
        ["Uniform sparse ($c=10$)", "Scale-free ($m_0=10$, $m=5$)"],
    ):
        sub = df[df["label"] == label]
        for variant, label_name, color in [
            ("luby", "Luby", LUBY_COLOR),
            ("improved", "Luby Improved", IMPROVED_COLOR),
        ]:
            rows = _thread_scaling_rows(sub, variant)
            ax.errorbar(
                rows["threads"],
                rows["mean_time_ms"],
                yerr=rows["graph_std_ms"],
                label=label_name,
                marker="o",
                capsize=3,
                color=color,
            )
        ax.set_xlabel("Threads")
        ax.set_ylabel("Time (ms)")
        ax.set_title(title)
        ax.set_xticks([1, 2, 3, 4, 6, 8, 10, 12, 16])
        ax.legend(fontsize=8)
        ax.grid(True, alpha=0.3)
    fig.suptitle("§2: Thread scaling ($N=5\\times10^6$, CSR)", fontsize=11)
    fig.tight_layout()
    save_fig(fig, out_dir, "section2_thread_scaling")


def _topology_groups(df: pd.DataFrame) -> list[tuple[str, pd.DataFrame]]:
    groups = []
    for label in df["label"].unique():
        groups.append((label, df[df["label"] == label]))
    return groups


def plot_topology(df: pd.DataFrame, out_dir: Path, stem: str, title: str) -> None:
    groups = _topology_groups(df)
    fig, axes = plt.subplots(2, 1, figsize=(9, 7))
    families = [
        ("uniform_sparse", "Uniform sparse"),
        ("scale_free", "Scale-free"),
    ]
    algos = ["Sequential CSR", "Luby 16t CSR", "Luby Improved 16t CSR", "Luby GPU CSR"]
    short_names = ["Sequential", "Luby 16t", "Improved 16t", "GPU"]
    x_offset = [-0.3, -0.1, 0.1, 0.3]
    width = 0.18

    for ax, (family, fam_title) in zip(axes, families):
        fam_df = df[df["graph_family"] == family]
        labels = [g[0] for g in groups if g[1]["graph_family"].iloc[0] == family]
        x = range(len(labels))
        for i, (algo, short, dx) in enumerate(zip(algos, short_names, x_offset)):
            means, stds = [], []
            for lab in labels:
                row = fam_df[(fam_df["label"] == lab) & (fam_df["algorithm"] == algo)]
                if not row.empty:
                    means.append(row["mean_time_ms"].iloc[0])
                    stds.append(row["graph_std_ms"].iloc[0])
                else:
                    means.append(0)
                    stds.append(0)
            ax.bar(
                [xi + dx for xi in x],
                means,
                width,
                yerr=stds,
                capsize=2,
                label=short,
                color=ALGO_COLORS.get(algo, "#888888"),
            )
        ax.set_xticks(x, [_pretty_label(l) for l in labels], fontsize=8)
        ax.set_ylabel("Time (ms)")
        ax.set_title(fam_title)
        ax.legend(fontsize=8, ncol=4)
        ax.grid(axis="y", alpha=0.3)

    fig.suptitle(title, fontsize=11)
    fig.tight_layout()
    save_fig(fig, out_dir, stem)


def _pretty_label(label: str) -> str:
    if "_c" in label and label.startswith("uniform"):
        c = label.rsplit("_c", 1)[-1]
        return f"$c={c}$"
    if "_m" in label and label.startswith("scale_free"):
        m = label.rsplit("_m", 1)[-1]
        return f"$m={m}$"
    return label


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--results-dir",
        type=Path,
        default=DEFAULT_RESULTS,
        help="Directory containing section*.csv files",
    )
    parser.add_argument(
        "--out-dir",
        type=Path,
        default=DEFAULT_OUT,
        help="Output directory for figures",
    )
    args = parser.parse_args()

    print(f"Results: {args.results_dir}")
    print(f"Output:  {args.out_dir}")

    s0 = load_csv(args.results_dir, "section0_csr_justify")
    if s0 is not None:
        plot_section0(s0, args.out_dir)

    s1 = load_csv(args.results_dir, "section1_scaling_n")
    if s1 is not None:
        plot_section1(s1, args.out_dir)

    s2 = load_csv(args.results_dir, "section2_thread_scaling")
    if s2 is not None:
        plot_section2(s2, args.out_dir)

    s3 = load_csv(args.results_dir, "section3_topology")
    if s3 is not None:
        plot_topology(
            s3,
            args.out_dir,
            "section3_topology",
            "§3: Graph topology ($N=5\\times10^6$, 16 threads)",
        )

    s4 = load_csv(args.results_dir, "section4_summary")
    if s4 is not None:
        plot_topology(
            s4,
            args.out_dir,
            "section4_summary",
            "§4: Single-run ($N=5\\times10^6$, 50 graphs, 16 threads)",
        )
    print("Done.")


if __name__ == "__main__":
    main()
