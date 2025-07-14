#include <utils/wavelet.h>

wavelet_obj *wavelet_construct( unsigned int signalLength) {
  wavelet_obj *obj;
  wave_object wave_temp_obj; // Temporary for wave_init

  obj = (wavelet_obj *)malloc(sizeof(wavelet_obj));
  if (obj == NULL) return NULL;

  obj->signalLength = signalLength;
  obj->numLevels = 4;
  obj->waveletName = "db4";
  if (obj->waveletName == NULL) {
    free(obj);
    return NULL;
  }

  // Initialize a temporary wave_object to pass to wt_init
  wave_temp_obj = wave_init(obj->waveletName);
  if (wave_temp_obj == NULL) {
    free(obj->waveletName);
    free(obj);
    return NULL;
  }

  // Initialize the wt_object
  obj->wt_obj = wt_init(wave_temp_obj, "dwt", obj->signalLength, obj->numLevels);
  if (obj->wt_obj == NULL) {
    wave_free(wave_temp_obj); // Free the temporary wave_object
    free(obj->waveletName);
    free(obj);
    return NULL;
  }

  // Set default extension and convolution (as in the example)
  setDWTExtension(obj->wt_obj, "sym"); // Or "per"
  setWTConv(obj->wt_obj, "direct");

  // Get the actual coefficient length from the initialized wt_object
  obj->coeffLength = obj->wt_obj->outlength;

  // Allocate internal buffer for converting float input to double
  obj->internalInputBuffer =
    (double *)malloc(sizeof(double) * obj->signalLength);
  if (obj->internalInputBuffer == NULL) {
    wt_free(obj->wt_obj); // This also frees the underlying wave_object
    free(obj->waveletName);
    free(obj);
    return NULL;
  }

  return obj;
}

void wavelet_destroy(wavelet_obj *obj) {
  if (obj == NULL) {
    return;
  }

  free(obj->internalInputBuffer);
  free(obj->waveletName);
  wt_free(obj->wt_obj); // This also handles freeing the underlying wave_object
  free((void *)obj);
}

void wavelet_dwt(wavelet_obj *obj, const float *in, float *out) {
  unsigned int i;

  for (i = 0; i < obj->signalLength; ++i) {
    obj->internalInputBuffer[i] = (double)in[i];
  }

  dwt(obj->wt_obj, obj->internalInputBuffer);

  for (i = 0; i < obj->coeffLength; ++i) {
    out[i] = (float)obj->wt_obj->output[i];
  }
}

void wavelet_idwt(wavelet_obj *obj, const float *in, float *out) {
  unsigned int i;

  for (i = 0; i < obj->coeffLength; ++i) {
    obj->wt_obj->output[i] = (double)in[i];
  }

  idwt(obj->wt_obj, obj->wt_obj->output);

  for (i = 0; i < obj->signalLength; ++i) {
    out[i] = (float)obj->wt_obj->output[i];
  }
}
