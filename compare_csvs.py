import pandas as pd
import os

files = ["section1_scaling_n.csv", "section2_thread_scaling.csv", "section3_topology.csv"]

for f in files:
    root_path = f
    result_path = os.path.join("result", f)
    
    if not os.path.exists(root_path) or not os.path.exists(result_path):
        print(f"Skipping {f}, file missing.")
        continue
        
    df_root = pd.read_csv(root_path)
    df_result = pd.read_csv(result_path)
    
    print(f"\n--- Comparison for {f} ---")
    print(f"Rows: {len(df_root)} (root) vs {len(df_result)} (result)")
    
    # Check if they have same structure and labels
    if list(df_root['label']) != list(df_result['label']) or list(df_root['algorithm']) != list(df_result['algorithm']):
         print("Warning: Labels or algorithms do not match perfectly.")
    
    avg_std_root = df_root['graph_std_ms'].mean()
    avg_std_result = df_result['graph_std_ms'].mean()
    
    avg_run_std_root = df_root['mean_run_std_ms'].mean()
    avg_run_std_result = df_result['mean_run_std_ms'].mean()
    
    print(f"Avg graph_std_ms: {avg_std_root:.4f} (root) vs {avg_std_result:.4f} (result)")
    print(f"Avg mean_run_std_ms: {avg_run_std_root:.4f} (root) vs {avg_run_std_result:.4f} (result)")
    
    reduction_graph = (avg_std_result - avg_std_root) / avg_std_result * 100 if avg_std_result > 0 else 0
    reduction_run = (avg_run_std_result - avg_run_std_root) / avg_run_std_result * 100 if avg_run_std_result > 0 else 0
    
    print(f"Noise reduction: {reduction_graph:.1f}% (graph), {reduction_run:.1f}% (run)")
    
    # Specific check for GPU if present
    gpu_root = df_root[df_root['algorithm'].str.contains('GPU', na=False)]
    gpu_result = df_result[df_result['algorithm'].str.contains('GPU', na=False)]
    
    if not gpu_root.empty:
        gpu_avg_std_root = gpu_root['graph_std_ms'].mean()
        gpu_avg_std_result = gpu_result['graph_std_ms'].mean()
        print(f"GPU Avg graph_std_ms: {gpu_avg_std_root:.4f} (root) vs {gpu_avg_std_result:.4f} (result)")
