package com.yingke.yuv2rgb

import android.util.Log
import java.io.File
import kotlin.experimental.and

const val RGB_TAG = "RGB_YUV"

fun test(path: String, dest: String) {
    val file = File(path)
    val start = System.currentTimeMillis()
    val yuv = rgb24_to_yuv420p(file.readBytes(), 510, 510)
    val end = System.currentTimeMillis() - start
    Log.d(RGB_TAG, "耗时: $end")
    val dstFile = File(dest)
    dstFile.writeBytes(yuv)
}

fun rgb24_to_yuv420p(rgb24: ByteArray, width: Int, height: Int): ByteArray {
    Log.d(RGB_TAG, "rgb24 len = ${rgb24.size}, width * height * 3 = ${width * height * 3}")
    val yuv420p = ByteArray(width * height * 3 / 2)

    var yIndex = 0
    var uIndex = width * height
    var vIndex = width * height * 5 / 4

    var index: Int
    var y: Int
    var u: Int
    var v: Int
    var r: Int
    var g: Int
    var b: Int
    for (j in 0 until height) {
        for (i in 0 until width) {
            index = width * j * 3 + i * 3

            r = rgb24[index].toInt()
            g = rgb24[index + 1].toInt()
            b = rgb24[index + 2].toInt()

            y = ((66 * r + 129 * g + 24 * b + 128) shr 8) + 16
            u = ((-38 * r - 74 * g + 112 * b + 128) shr 8) + 128
            v = ((112 * r - 94 * g - 18 * b + 128) shr 8) + 128

            yuv420p[yIndex++] = clipValue(y, 0, 255)
            if (j % 2 == 0 && i % 2 == 0) {
                yuv420p[uIndex++] = clipValue(u, 0, 255)
            } else if(i % 2 == 0) {
                yuv420p[vIndex++] = clipValue(v, 0, 255)
            }
        }
    }
    return yuv420p
}

fun clipValue(x: Int, min: Int, max: Int): Byte {
    val res = if (x > max) {
        max
    } else if (x < min) {
        min
    } else {
        x
    }
    return res.toByte()
}