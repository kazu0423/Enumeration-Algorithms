#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import argparse
import math
import os
import random
import sys

try:
    import matplotlib
    matplotlib.use("Agg")
    import matplotlib.pyplot as plt
except ImportError as exc:
    print("matplotlib が見つかりません。`pip install matplotlib` で入れてください。", file=sys.stderr)
    raise


def load_transactions(path, max_transactions=None):
    transactions = []
    max_item = -1
    with open(path, "r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            items = [int(x) for x in line.split() if x.strip() != ""]
            if not items:
                continue
            items = sorted(set(items))
            max_item = max(max_item, items[-1])
            transactions.append(items)
            if max_transactions and len(transactions) >= max_transactions:
                break
    n_items = max_item + 1 if max_item >= 0 else 0
    return transactions, n_items


def zscore(values):
    if not values:
        return []
    mean = sum(values) / len(values)
    var = sum((v - mean) ** 2 for v in values) / len(values)
    std = math.sqrt(var) if var > 0 else 1.0
    return [(v - mean) / std for v in values]


def kmeans(points, k, max_iters=50, seed=0):
    if not points:
        return [], []
    k = max(1, min(k, len(points)))
    random.seed(seed)
    centers = random.sample(points, k)
    labels = [0] * len(points)
    for _ in range(max_iters):
        changed = False
        clusters = [[] for _ in range(k)]
        for i, p in enumerate(points):
            best = min(range(k), key=lambda j: (p[0] - centers[j][0]) ** 2 + (p[1] - centers[j][1]) ** 2)
            if labels[i] != best:
                changed = True
            labels[i] = best
            clusters[best].append(p)
        new_centers = []
        for j in range(k):
            if clusters[j]:
                sx = sum(p[0] for p in clusters[j])
                sy = sum(p[1] for p in clusters[j])
                new_centers.append((sx / len(clusters[j]), sy / len(clusters[j])))
            else:
                new_centers.append(centers[j])
        centers = new_centers
        if not changed:
            break
    return labels, centers


def main():
    parser = argparse.ArgumentParser(description="トランザクションの密度を可視化します。")
    parser.add_argument("input", help=".dat 形式の入力ファイル")
    parser.add_argument("-o", "--output", default="viz/transactions.png", help="出力画像パス")
    parser.add_argument("-k", "--clusters", type=int, default=3, help="クラスタ数")
    parser.add_argument("--max-transactions", type=int, default=0, help="先頭から読む最大トランザクション数")
    args = parser.parse_args()

    max_tx = args.max_transactions if args.max_transactions > 0 else None
    transactions, n_items = load_transactions(args.input, max_tx)
    if not transactions or n_items == 0:
        print("有効なトランザクションがありません。", file=sys.stderr)
        return 1

    m = len(transactions)
    freq = [0] * n_items
    for t in transactions:
        for item in t:
            freq[item] += 1
    freq_norm = [f / m for f in freq]

    lengths = []
    densities = []
    avg_item_freqs = []
    for t in transactions:
        length = len(t)
        density = length / n_items if n_items > 0 else 0.0
        avg_freq = sum(freq_norm[item] for item in t) / length if length > 0 else 0.0
        lengths.append(length)
        densities.append(density)
        avg_item_freqs.append(avg_freq)

    zlen = zscore(lengths)
    zfreq = zscore(avg_item_freqs)
    points = list(zip(zlen, zfreq))
    labels, _ = kmeans(points, args.clusters)

    k = max(labels) + 1 if labels else 0
    cluster_sizes = [0] * k
    cluster_density = [0.0] * k
    for i, lbl in enumerate(labels):
        cluster_sizes[lbl] += 1
        cluster_density[lbl] += densities[i]
    for i in range(k):
        if cluster_sizes[i] > 0:
            cluster_density[i] /= cluster_sizes[i]

    os.makedirs(os.path.dirname(args.output) or ".", exist_ok=True)

    fig, axes = plt.subplots(1, 3, figsize=(15, 4))
    axes[0].hist(densities, bins=30, color="#4c78a8", edgecolor="black")
    axes[0].set_title("Transaction Density Histogram")
    axes[0].set_xlabel("density (|T| / n_items)")
    axes[0].set_ylabel("count")

    scatter = axes[1].scatter(lengths, avg_item_freqs, c=labels, s=8, cmap="tab10", alpha=0.7)
    axes[1].set_title("Length vs Avg Item Freq")
    axes[1].set_xlabel("transaction length")
    axes[1].set_ylabel("avg item frequency")
    if labels:
        legend = axes[1].legend(*scatter.legend_elements(), title="cluster", loc="best", fontsize="small")
        axes[1].add_artist(legend)

    axes[2].bar(range(k), cluster_sizes, color="#f28e2b", edgecolor="black")
    axes[2].set_title("Cluster Sizes")
    axes[2].set_xlabel("cluster id")
    axes[2].set_ylabel("count")

    fig.suptitle(f"Transactions: {m}, Items: {n_items}")
    fig.tight_layout(rect=[0, 0, 1, 0.94])
    fig.savefig(args.output, dpi=150)
    print(f"saved: {args.output}")
    print("cluster densities:", " ".join(f"{i}:{cluster_density[i]:.4f}" for i in range(k)))
    return 0


if __name__ == "__main__":
    sys.exit(main())
