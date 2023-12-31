/*
 * Copyright 2009-2011 Cedric Priscal
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//#include <termios.h>
#include <asm/termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <jni.h>
#include<time.h>
#include <errno.h>
#include <termios.h>
#include "com_flownex_libecono_SerialPort.h"

#include "android/log.h"

static const char *TAG = "serial_port";
#define LOGI(fmt, args...) __android_log_print(ANDROID_LOG_INFO,  TAG, fmt, ##args)
#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, TAG, fmt, ##args)
#define LOGE(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, TAG, fmt, ##args)

/*
 * Class:     android_serialport_SerialPort
 * Method:    open
 * Signature: (Ljava/lang/String;II)Ljava/io/FileDescriptor;
 */

#define ERROR -1
#define Success 0
#define Timeout -2
static speed_t getBaudrate(jint baudRate)
{
    switch(baudRate) {
        case 0: return B0;
        case 50: return B50;
        case 75: return B75;
        case 110: return B110;
        case 134: return B134;
        case 150: return B150;
        case 200: return B200;
        case 300: return B300;
        case 600: return B600;
        case 1200: return B1200;
        case 1800: return B1800;
        case 2400: return B2400;
        case 4800: return B4800;
        case 9600: return B9600;
        case 19200: return B19200;
        case 38400: return B38400;
        case 57600: return B57600;
        case 115200: return B115200;
        case 230400: return B230400;
        case 460800: return B460800;
        case 500000: return B500000;
        case 576000: return B576000;
        case 921600: return B921600;
        case 1000000: return B1000000;
        case 1152000: return B1152000;
        case 1500000: return B1500000;
        case 2000000: return B2000000;
        case 2500000: return B2500000;
        case 3000000: return B3000000;
        case 3500000: return B3500000;
        case 4000000: return B4000000;
        default: return -1;
    }
}

JNIEXPORT jobject JNICALL Java_com_flownex_libecono_SerialPort_open
        (JNIEnv *env, jclass thiz, jstring path, jint baudrate, jint stopBits, jint dataBits,
         jint parity, jint flowCon, jint flags) {
        int fd;
        speed_t speed;
        jobject mFileDescriptor;

        /* Check arguments */
        {
            speed = getBaudrate(baudrate);
            if (speed == -1) {
                /* TODO: throw an exception */
                LOGE("Invalid baudrate");
                return NULL;
            }
        }

        /* Opening device */
        {
            jboolean iscopy;
            const char *path_utf = (*env)->GetStringUTFChars(env, path, &iscopy);
            LOGD("Opening serial port %s with flags 0x%x", path_utf, O_RDWR | flags);
            fd = open(path_utf, O_RDWR | flags);
            LOGD("open() fd = %d", fd);
            (*env)->ReleaseStringUTFChars(env, path, path_utf);
            if (fd == -1) {
                /* Throw an exception */
                LOGE("Cannot open port");
                /* TODO: throw an exception */
                return NULL;
            }
        }

        /* Configure device */
        {
            struct termios2 cfg;

            ioctl (fd, TCGETS2, &cfg);
            // Set baud rate
            cfg.c_cflag &= ~CBAUD;
            cfg.c_cflag |= BOTHER ;
            cfg.c_cflag = CLOCAL | speed | CREAD;
            cfg.c_oflag = 0;
            cfg.c_lflag = 0;

//            cfg.c_ispeed = baudrate;
//            cfg.c_ospeed = baudrate;

            cfg.c_cflag &= ~CSIZE;
            switch (dataBits) {
                case 5:
                    cfg.c_cflag |= CS5;    //5 Data Bits
                    break;
                case 6:
                    cfg.c_cflag |= CS6;    //6 Data Bits
                    break;
                case 7:
                    cfg.c_cflag |= CS7;    //7 Data Bits
                    break;
                case 8:
                    cfg.c_cflag |= CS8;    //8 Data Bits
                    break;
                default:
                    cfg.c_cflag |= CS8;
                    break;
            }

            switch (parity) {
                case 0:
                    cfg.c_cflag &= ~PARENB;    //PARITY OFF
                    break;
                case 1:
                    cfg.c_cflag |= (PARODD | PARENB);   //PARITY ODD
                    cfg.c_iflag &= ~IGNPAR;
                    cfg.c_iflag |= PARMRK;
                    cfg.c_iflag |= INPCK;
                    break;
                case 2:
                    cfg.c_iflag &= ~(IGNPAR | PARMRK); //PARITY EVEN
                    cfg.c_iflag |= INPCK;
                    cfg.c_cflag |= PARENB;
                    cfg.c_cflag &= ~PARODD;
                    break;
                case 3:
                    //  PARITY SPACE
                    cfg.c_iflag &= ~IGNPAR;             //  Make sure wrong parity is not ignored
                    cfg.c_iflag |= PARMRK;              //  Marks parity error, parity error
                    //  is given as three char sequence
                    cfg.c_iflag |= INPCK;               //  Enable input parity checking
                    cfg.c_cflag |= PARENB | CMSPAR;     //  Enable parity and set space parity
                    cfg.c_cflag &= ~PARODD;             //
                    break;
                case 4:
                    //  PARITY MARK
                    cfg.c_iflag &= ~IGNPAR;             //  Make sure wrong parity is not ignored
                    cfg.c_iflag |= PARMRK;              //  Marks parity error, parity error
                    //  is given as three char sequence
                    cfg.c_iflag |= INPCK;               //  Enable input parity checking
                    cfg.c_cflag |= PARENB | CMSPAR | PARODD;
                    break;
                default:
                    cfg.c_cflag &= ~PARENB;
                    break;
            }

            switch (stopBits) {
                case 1:
                    cfg.c_cflag &= ~CSTOPB;    //1 Stop Bit
                    break;
                case 2:
                    cfg.c_cflag |= CSTOPB;    //2 Stop Bits
                    break;
                default:
                    cfg.c_cflag &= ~CSTOPB;
                    break;
            }

            // hardware flow control
            switch (flowCon) {
                case 0:
                    cfg.c_cflag &= ~CRTSCTS;    //No Flow Control
                    break;
                case 1:
                    cfg.c_cflag |= CRTSCTS;    //Hardware Flow Control
                    break;
                case 2:
                    cfg.c_cflag |= IXON | IXOFF | IXANY;    //Software Flow Control
                    break;
                default:
                    cfg.c_cflag &= ~CRTSCTS;
                    break;
            }
           tcflush(fd, TCIFLUSH);
        LOGD("setting port");
            if (ioctl(fd, TCSETS2, &cfg)) {
                LOGE("tcsets2() failed");
                close(fd);
                /* TODO: throw an exception */
                return NULL;
            }
        }

        /* Create a corresponding file descriptor */
        {
            jclass cFileDescriptor = (*env)->FindClass(env, "java/io/FileDescriptor");
            jmethodID iFileDescriptor = (*env)->GetMethodID(env, cFileDescriptor, "<init>", "()V");
            jfieldID descriptorID = (*env)->GetFieldID(env, cFileDescriptor, "descriptor", "I");
            mFileDescriptor = (*env)->NewObject(env, cFileDescriptor, iFileDescriptor);
            (*env)->SetIntField(env, mFileDescriptor, descriptorID, (jint) fd);
        }

        return mFileDescriptor;
}

JNIEXPORT jint JNICALL Java_com_flownex_libecono_SerialPort_serial_1read
        (JNIEnv *env, jobject thiz, jobject fd, jbyteArray buf , jint size , jint T)
{

    jint descriptor = -1;

    jclass SerialPortClass = (*env)->GetObjectClass(env, thiz);
    jclass FileDescriptorClass = (*env)->FindClass(env, "java/io/FileDescriptor");
    jfieldID mFdID = (*env)->GetFieldID(env, SerialPortClass, "mFd", "Ljava/io/FileDescriptor;");
    jfieldID descriptorID = (*env)->GetFieldID(env, FileDescriptorClass, "descriptor", "I");
    jobject fd2 = (*env)->GetObjectField(env, thiz, mFdID);

    descriptor = (*env)->GetIntField(env, fd2, descriptorID);
    LOGD("descriptor = %d", descriptor);

    jbyte *buffer = (*env)->GetByteArrayElements(env, buf, NULL);

    jbyte *resbuf;
    fd_set set;

    struct timeval timeout = {0, T};
    int ready=0;
    int TotalBytes=0;
    int r = 0;
    int expected_Byte=size;
    int filedisc = descriptor;
    int sizetoread = 0;
    static int ntoread=0;

    LOGD("descriptor2  = %d", descriptor);

    FD_ZERO(&set);  /* clear the set */
    FD_SET(filedisc, &set); /* add our file descriptor to the set */


    double sec = ((double)T/(double)1000000);
    LOGD(" sec %lf",sec);

    ready = select(descriptor + 1 , &set, NULL, NULL, &timeout);
    LOGD("errno = %s",strerror(errno));
    if(ready == -1){
        /* Some error has occured in input */
        LOGD(" ERROR");
        return ERROR;
    }

    if(ready == 0){
        /* a timeout occured */
        LOGD(" TIMEOUT occured");

        return Timeout;
    }
    else if(ready){
        clock_t  total_t;
        /* there was data to read */
        LOGD(" reading ");

        TotalBytes = read(descriptor, buffer, expected_Byte);
        LOGD(" first %d", TotalBytes);
        if (TotalBytes < expected_Byte){
            LOGD(" inside");
            sizetoread = expected_Byte - TotalBytes;
            ntoread = sizetoread;

            total_t = clock() + sec * CLOCKS_PER_SEC;
            while (clock() < total_t) {

                ntoread = ntoread - r;
                double time = ((double)total_t - (double)clock())/ (double)CLOCKS_PER_SEC;
                LOGD(" time %lf",time );
                LOGD(" ntoread %d",ntoread );
                struct timeval t = {0, time};
                if(select(descriptor + 1 , &set, NULL, NULL, &t)>0){
                    r = read(descriptor, &buffer[TotalBytes], ntoread);
                    LOGD(" r %d", r);
                    TotalBytes += r;

                    LOGD(" TotalBytes %d", TotalBytes);

                }else{
                    LOGD(" break1 ");
                    break;
                }
            }
        }
    }
    if(TotalBytes == 0){
        LOGD("TIMEOUT TOTALBYTE = 0");
        return Timeout;
    }
    (*env)->SetByteArrayRegion(env, buf, 0,size , buffer);
    LOGD(" TotalBytes %d", TotalBytes);
    FD_CLR(filedisc, &set);
    return TotalBytes;

}


JNIEXPORT jint JNICALL Java_com_flownex_libecono_SerialPort_serial_1write
        (JNIEnv *env , jobject thiz, jobject fd , jbyteArray write_buffer, jint size)
{
    jint descriptor = -1;

    jclass SerialPortClass = (*env)->GetObjectClass(env, thiz);
    jclass FileDescriptorClass = (*env)->FindClass(env, "java/io/FileDescriptor");
    jfieldID mFdID = (*env)->GetFieldID(env, SerialPortClass, "mFd", "Ljava/io/FileDescriptor;");
    jfieldID descriptorID = (*env)->GetFieldID(env, FileDescriptorClass, "descriptor", "I");
    jobject fd2 = (*env)->GetObjectField(env, thiz, mFdID);

    descriptor = (*env)->GetIntField(env, fd2, descriptorID);

    jbyte *buffer = (*env)->GetByteArrayElements(env, write_buffer, NULL);
    LOGD("descriptor = %d", descriptor);

    //unsigned char cmd[] = {0xAA, 0x66, 0x00, 0x04, 0x37, 0x00, 0x3B};
    int n;
    n = write(descriptor, buffer, size);
    // int TotalBytes = read(descriptor, buffer2, 1000);
    LOGD(" n %d", n);
    LOGD(" inside write %d", size);
    if(n != (size))
    {
        LOGD(" inside write if %d", n);
        if(errno == EAGAIN)
            return ERROR;
    }else{
        LOGD(" inside write else %d", n);
        LOGD("buffer written %s",buffer);
        (*env)->SetByteArrayRegion(env, write_buffer, 0,size , buffer);
        return n;
    }

}


/*
 * Class:     cedric_serial_SerialPort
 * Method:    close
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_flownex_libecono_SerialPort_close
(JNIEnv *env, jobject thiz) {
jclass SerialPortClass = (*env)->GetObjectClass(env, thiz);
jclass FileDescriptorClass = (*env)->FindClass(env, "java/io/FileDescriptor");

jfieldID mFdID = (*env)->GetFieldID(env, SerialPortClass, "mFd", "Ljava/io/FileDescriptor;");
jfieldID descriptorID = (*env)->GetFieldID(env, FileDescriptorClass, "descriptor", "I");

jobject mFd = (*env)->GetObjectField(env, thiz, mFdID);
jint descriptor = (*env)->GetIntField(env, mFd, descriptorID);

LOGD("close(fd = %d)", descriptor);
close(descriptor);
}

