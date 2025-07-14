#ifndef __ODAS_UTILS_WAVELET_H
#define __ODAS_UTILS_WAVELET_H

#include <wavelib.h>
#include <stdlib.h>
#include <string.h>

//! Wavelet object.
typedef struct wavelet_obj {
  unsigned int signalLength; ///< Original signal length (in samples).
  unsigned int numLevels;    ///< Number of decomposition levels (J).
  unsigned int coeffLength;  ///< Total length of the output coefficient array.
  char *waveletName;         ///< Name of the wavelet (e.g., "db4").

  // The primary wavelib object, encapsulating the transform details
  wt_object wt_obj;

  // Internal buffer for float -> double conversion of input signal
  double *internalInputBuffer;

} wavelet_obj;

/**
 * \brief Constructor. Initializes all fields to perform DWT.
 * \param signalLength  The length of the signal to be transformed.
 * \return              Pointer to the instantiated object.
 */
wavelet_obj *wavelet_construct(unsigned int signalLength);

/**
 * \brief Destructor. Frees memory associated with the wavelet object.
 * \param obj           Object to be destroyed.
 */
void wavelet_destroy(wavelet_obj *obj);

/**
 * \brief Perform forward Discrete Wavelet Transform (DWT).
 * \param obj           Object used for the transform.
 * \param in            Pointer to the real float array used for input.
 * \param out           Pointer to the real float array for output coefficients.
 *                      The output format is [A(J) D(J) D(J-1)...D(1)].
 */
void wavelet_dwt(wavelet_obj *obj, const float *in, float *out);

/**
 * \brief Perform inverse Discrete Wavelet Transform (IDWT).
 * \param obj           Object used for the transform.
 * \param in            Pointer to the real float array of coefficients for input.
 * \param out           Pointer to the real float array for the reconstructed signal.
 */
void wavelet_idwt(wavelet_obj *obj, const float *in, float *out);

#endif
