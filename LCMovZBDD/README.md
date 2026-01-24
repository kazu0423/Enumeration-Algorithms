# LCM (Closed Frequent Itemset Mining) with MPI

このプログラムは逆探索ベースの LCM アルゴリズムで頻出飽和アイテム集合
(closed frequent itemset) を列挙します。OpenMPI でトップレベルの拡張を
分割して並列化しています。

## 入力形式

- `data` ディレクトリ内のデータ形式に合わせています。
- 1 行に 1 トランザクション。
- 各行は空白区切りの非負整数アイテム ID の列。
- ヘッダ行はありません。
- 空行は無視します。

例:

```
0 2 3 10
1 4 10
0 1 2 4
```

## 実行方法

ビルド:

```
make
```

実行 (MPI 並列):

```
mpirun -np 4 ./LCM data/accidents/ac_30k.dat
```

密度に応じたモード切替 (auto がデフォルト):

```
mpirun -np 4 ./LCM data/bms2/bms2_800.dat --mode auto
mpirun -np 4 ./LCM data/bms2/bms2_800.dat --mode dense
mpirun -np 4 ./LCM data/bms2/bms2_800.dat --mode sparse
```

実行後、標準入力で `minsup` (最小支持度) を入力します。

単一プロセスで動かす場合は `mpirun` なしでも動作します:

```
./LCM data/accidents/ac_30k.dat
```

逐次出力モード (見つけるたびに出力):

```
mpirun -np 4 ./LCM data/accidents/ac_30k.dat --stream
```

## 出力形式

LCM 形式で、各行が

```
item1 item2 ... itemk (support)
```

となります。出力の先頭に空集合の閉包が来ます。空集合の場合は
` (m)` のように表示されます。

## 注意

- アイテム ID は 0 以上の整数を想定しています。
- MPI での出力は rank 0 に集約して表示します。
- `--stream` を使うと見つけ次第出力するため、MPI 実行では出力が
  混ざる可能性があります。
- `--mode auto` は平均密度 (平均取引長 / アイテム数) が 0.1 以上なら
  dense、それ未満なら sparse を選びます。
