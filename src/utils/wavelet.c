#include <utils/wavelet.h>

wavelet_obj *wavelet_construct(unsigned int signalLength) {
  wavelet_obj *obj;

  obj = (wavelet_obj *)malloc(sizeof(wavelet_obj));
  if (obj == NULL) {
    return NULL;
  }

  // Initialize members to NULL for safer cleanup in case of failure
  obj->waveletName = NULL;
  obj->wave_obj = NULL;
  obj->wt_obj = NULL;
  obj->internalInputBuffer = NULL;
  obj->internalOutputBuffer = NULL;

  obj->signalLength = signalLength;
  obj->numLevels = 2;
  obj->waveletName = strdup("db4");
  if (obj->waveletName == NULL) {
    wavelet_destroy(obj); // Use the destructor for cleanup
    return NULL;
  }

  // 1. Initialize the wave_object and store it in our struct
  obj->wave_obj = wave_init(obj->waveletName);
  if (obj->wave_obj == NULL) {
    wavelet_destroy(obj);
    return NULL;
  }

  // 2. Initialize the wt_object using our stored wave_object
  obj->wt_obj =
    wt_init(obj->wave_obj, "modwt", obj->signalLength, obj->numLevels);
  if (obj->wt_obj == NULL) {
    wavelet_destroy(obj);
    return NULL;
  }

  setDWTExtension(obj->wt_obj, "per");
  setWTConv(obj->wt_obj, "direct");

  obj->coeffLength = obj->wt_obj->outlength;

  obj->internalInputBuffer =
    (double *)malloc(sizeof(double) * obj->signalLength);
  if (obj->internalInputBuffer == NULL) {
    wavelet_destroy(obj);
    return NULL;
  }

  obj->internalOutputBuffer =
    (double *)malloc(sizeof(double) * obj->signalLength);
  if (obj->internalOutputBuffer == NULL) {
    wavelet_destroy(obj);
    return NULL;
  }

  return obj;
}

void wavelet_destroy(wavelet_obj *obj) {
  if (obj == NULL) {
    return;
  }

  free(obj->internalInputBuffer);
  free(obj->internalOutputBuffer);
  free(obj->waveletName);
  wave_free(obj->wave_obj);
  wt_free(obj->wt_obj);
  free(obj);
}

void wavelet_dwt(wavelet_obj *obj, const float *in, float *out) {
  unsigned int i;

  for (i = 0; i < obj->signalLength; ++i) {
    obj->internalInputBuffer[i] = (double)in[i];
  }

  modwt(obj->wt_obj, obj->internalInputBuffer);

  for (i = 0; i < obj->signalLength; ++i) {
    out[i] = (float)obj->wt_obj->output[i];
  }
}

void wavelet_idwt(wavelet_obj *obj, const float *in, float *out) {
  unsigned int i;

  for (i = 0; i < obj->signalLength; ++i) {
    obj->wt_obj->output[i] = (double)in[i];
  }

  imodwt(obj->wt_obj, obj->internalOutputBuffer);

  for (i = 0; i < obj->signalLength; ++i) {
    out[i] = (float)obj->internalOutputBuffer[i];
  }
}
