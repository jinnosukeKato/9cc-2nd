# 参考にした読み物一覧

## 教科書

- 低レイヤを知りたい人のためのCコンパイラ作成入門
    - https://www.sigbus.info/compilerbook

## 参考文献

- Codebug | sunadarakeのtech blog
  - segmentation fault 11(core dumped)の原因と2つのチェックポイント
  - https://code-bug.net/entry/2019/05/24/112315/
  - gdbの使い方(segmentation faultの原因解析手法)について
  - 10章で詰まったときに役立った
- Zenn
  - nac-39 compilerbook作成日記
  - https://zenn.dev/nac_39/scraps/b431e81e7d19c2
  - 10章で詰まったときに解決した(俺の思考がロックしていた)
  - 先駆者のブログとして大いに参考にした

## 詰まったところ

### if, while実装時

```c
a = 100;

if (0)
  0;

return a;
```

上記のような制御構文内の行が実行されないときにaの値（例であれば返り値）が不定になる．

#### 原因

コンパイルされたコードは次のようになっていた．

```asm
# ↓は前の行の結果をpopする
pop rax

～制御構文をコンパイルしたコード～

pop rax
```
このとき，制御構文内の行が実行されないと，`pop rax`が二度続くことになり，スタックが破壊されることが原因．

#### 解決方法

制御構文のアセンブリを出力する前に，`push rax`をすることで`pop rax`が連続になることを防いだ．  
今のところ問題はなさそうなのでこれが正しい対処法であることを願ってみる．

### for, whileとかラベルでジャンプする制御構文

#### 原因

条件式など，空のパターンがあり得る式の評価結果をgetする`pop rax`によってスタックが破壊されていた．

#### 解決方法

評価式が生成されないときはpopしないようにした．

`pop rax`が原因でバグってる回数が圧倒的に多いのでそのパターンに気づけるようになってきた．

### ブロックの導入

複数行の情報をどのように持たせる？→単方向リストがいいよね  
→Nodeにnextを持たせる

### 関数呼び出し

1. 返り値が変な値になっていた
1. printfを含むオブジェクトファイルをリンクするとseg faultする

#### 原因

1. スタックが破壊されていた
1. スタックを16 bitでalignmentしていなかった

#### 解決方法

[この方の解決法](https://github.com/takoeight0821/hoc_nyan/commit/d4d460948d12f7cf4f6e05a433dadfc9a5d8c647)を使った

`and rsp, -16`を`call`前で呼ぶことで，強制的に16以上の桁が1で埋められて，切り下げられる．

ChatGPTに聞いたらこの解説が帰ってきて分かりやすかった．

```java
If rsp = 0x7FFF_FFF8, then:
rsp AND -16 = 0x7FFF_FFF0

If rsp = 0x7FFF_FFFD, then:
rsp AND -16 = 0x7FFF_FFF0
```
