# トランザクション可視化ツール

`.dat` 形式のトランザクションを読み込み、密度分布と簡易クラスタリングを
画像として出力します。

## 依存

- Python 3
- matplotlib

```
pip3 install matplotlib
```

## 使い方

```
python3 viz/visualize_transactions.py data/bms2/bms2_800.dat
```

出力先を指定する場合:

```
python3 viz/visualize_transactions.py data/bms2/bms2_800.dat -o viz/bms2_800.png
```

クラスタ数を変更する場合:

```
python3 viz/visualize_transactions.py data/bms2/bms2_800.dat -k 4
```

先頭から読むトランザクション数を制限する場合:

```
python3 viz/visualize_transactions.py data/bms2/bms2_800.dat --max-transactions 5000
```

## 出力内容

- **密度ヒストグラム**: `|T| / n_items` の分布
- **散布図**: 取引長 vs 平均アイテム頻度 (クラスタ色分け)
- **クラスタサイズ**: k-means の各クラスタの件数

クラスタの平均密度は標準出力に表示します。
