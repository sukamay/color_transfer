cpp implementation based on "Color transfer between images"

## results
<figure>
  <img src="./concat_results/concat_001_002.png" width=100%/>
  <img src="./concat_results/concat_006_003.png" width=100%/>
  <img src="./concat_results/concat_008_010.png" width=100%/>
<br>
</figure>

## bad case
<figure>
  <img src="./concat_results/concat_009_000.png" width=100%/>
  <img src="./concat_results/concat_013_014.png" width=100%/>
</figure>


## usage

```shell
./ColorTransfer -source [source img pth] -target [target img path] -space [LAB|CAM97|RGB|OPENCV_LAB] -output [save dst img to output path, if no input, just show and not save]
```
if you wanna know more about the usage:
```shell
./ColorTransfer --help
ColorTransfer: Warning: SetUsageMessage() never called

  Flags from color_transfer/src/main.cpp:
    -cluster (if transfer color with cluster algorithm) type: bool
      default: false
    -k (k cluster if using cluster mode) type: int32 default: 0
    -output (output img path) type: string default: ""
    -source (source img path) type: string default: ""
    -space (color space:LAB|CAM97|RGB|OPENCV_LAB) type: string default: "LAB"
    -target (target img path) type: string default: ""
```

## Reference
> Reinhard E, Adhikhmin M, Gooch B, et al. Color transfer between images[J]. IEEE Computer graphics and applications, 2001, 21(5): 34-41.

## contact information
welcome to contact with me (sukamayluo@sjtu.edu.cn) if you have any questions.