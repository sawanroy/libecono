/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_flownex_libecono_SerialPort */

#ifndef _Included_com_flownex_libecono_SerialPort
#define _Included_com_flownex_libecono_SerialPort
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_flownex_libecono_SerialPort
 * Method:    open
 * Signature: (Ljava/lang/String;IIIIII)Ljava/io/FileDescriptor;
 */
JNIEXPORT jobject JNICALL Java_com_flownex_libecono_SerialPort_open
  (JNIEnv *env, jclass thiz, jstring path, jint baudrate, jint stopBits, jint dataBits,
           jint parity, jint flowCon, jint flags);

/*
 * Class:     com_flownex_libecono_SerialPort
 * Method:    close
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_flownex_libecono_SerialPort_close
  (JNIEnv *env, jobject thiz);

#ifdef __cplusplus
}
#endif
#endif
