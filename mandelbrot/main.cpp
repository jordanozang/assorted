#include <cassert>
#include <cstdint>

struct pxl {
  int8_t b, g, r;
};

struct cmplx {
  float re, im;
};

constexpr cmplx sqr(cmplx val) {
  cmplx res{};
  res.re = val.re * val.re - val.im * val.im;
  res.im = 2.0f * val.re * val.im;
  return res;
}

constexpr float nrm(cmplx val) { return val.re * val.re + val.im * val.im; }

constexpr cmplx sub(cmplx left, cmplx right) {
  cmplx res{};
  res.re = left.re - right.re;
  res.im = left.im - right.im;
  return res;
}

constexpr cmplx mndl_recurse(cmplx last, cmplx c) {
  cmplx res{};
  res.re = last.re * last.re - last.im * last.im + c.re;
  res.im = 2.0f * last.re * last.im + c.im;
  return res;
}

pxl CheckOne(cmplx c) {
  constexpr float nrm_bnd{2.1f};
  constexpr int max_iters{50};

  cmplx curr{0.0f, 0.0f};
  int iters{0};

  while (iters < max_iters && nrm(curr) < nrm_bnd) {
    curr = mndl_recurse(curr, c);

    ++iters;
  }

  if (nrm(curr) < nrm_bnd)
    return {0, 100, 0};
	
  float intensity = static_cast<float>(iters) / static_cast<float>(max_iters);

  return {static_cast<int8_t>(intensity * 127), 0, 0};
}

#include <png.h>

int write_png(pxl *canvas, int dim, const char *out_file) {
  png_image image{};
  image.version = PNG_IMAGE_VERSION;
  image.format = PNG_FORMAT_BGR;
  image.height = dim;
  image.width = dim;
  int ret = 0;
  if (png_image_write_to_file(&image, out_file, 0 /*convert_to_8bit*/, canvas,
                              0 /*row_stride*/, NULL /*colormap*/) == 0) {
    printf("write to '%s' failed:%s\n", out_file, image.message);
    ret = -1;
  }
  return ret;
}

int main() {

  int dim{5000};
  assert(dim % 4 == 0);

  pxl *canvas = new pxl[dim * dim];

  for (int i = 0; i < dim; ++i) {
    for (int j = 0; j < dim; ++j) {
      cmplx c;
      c.im = 4.0f * static_cast<float>(i) / static_cast<float>(dim) - 2.0f;
      c.re = 4.0f * static_cast<float>(j) / static_cast<float>(dim) - 2.0f;
      canvas[i * dim + j] = CheckOne(c);
    }
  }

  write_png(canvas, dim, "frac.png");

  delete[] canvas;

  return 0;
}
