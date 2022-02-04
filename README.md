# [Find image with best quality in given list](https://yduf.github.io/image-quality/)

Licensed under the [GNU General Public License, version 3. (GPLv3)](https://www.gnu.org/licenses/gpl-3.0.html)

This apply to a set of images picturing the same content, but that may have been altered or resized in a various way.
Based on following ideas to measure _image sharpness_ and _image noise_ of a set of images and compare their relatives quality.

## [Image sharpness](https://photo.stackexchange.com/a/117823/104580)

Image sharpness is evaluated by resizing all images to the bigger size & re-compress both images with exactly same quality preset, grayscale conversion, using JPEG or better algorithm.

## [Image noise](https://stackoverflow.com/a/25436112/51386)

Image noise is evaluated per image without resizing the image using a laplacian filter.
More detail and improved approach can be found in [1](https://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.173.1644&rep=rep1&type=pdf).

- [\[1\]](https://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.173.1644&rep=rep1&type=pdf) - A Fast Method For Image Noise Estimation Using
Laplacian Operator and Adaptive Edge Detection / Shen-Chuan Tai , Shih-Ming Yang

## Usage

```bash
$ ./bestq images/*
```

Output is a one line json string with the following format:

```json
[ // array of results
    {
        "file": "/path/to/image.jpg",     
        "sharpness": 0.7683959817912913,  // 1.0 is max sharpness (normalized)
        "jpg_size": 40005,                // used to evaluate absolute sharpness
        "noise": 1.0,                     // 1.0 is min noise (normalized)
        "sigma_noise": 0.5916566144368179 // absolute noise
    },
...
]
```

where _sharpness_ is relative to other images in the list, same for _noise_.

This information gives qualitative metrics one can use to estimate the quality of the underlying image data,
without relying only on the dimension of the images or their file size.

