#Grid grpah generatorの仕様

## コンパイル
g++ generator.cpp unionFind.cpp -o generator

## 使い方

- ./generator 縦 横 確率
- 出力: random縦_横_確率_ID.in

## 生成方法

## 出力の仕様

## 出力フォーマット
頂点数$n$ 辺数$m$  
$v_1$ $u_1$ $c_1$
$\vdots$  
$v_m$ $u_m$ $c_m$

ここで，$u_i, v_i$は$0$以上$n - 1$以下である．