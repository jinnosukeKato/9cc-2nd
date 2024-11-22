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
