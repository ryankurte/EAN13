/**
 * EAN13 Barcode Generator
 * Creates a boolean array corresponding to the EAN13 barcode for a string of
 * 12 numeric only characters.
 * From: https://github.com/ryankurte/ean13
 * Based on: https://github.com/astrokin/EAN13BarcodeGenerator
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Ryan Kurte
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <stdbool.h>

#include "ean13.h"

static char test[] = "222543841541";

static bool test_code[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0,  //quiet
    1, 0, 1,                    //porch
    0, 0, 1, 0, 0, 1, 1,        //2 left_odd
    0, 0, 1, 0, 0, 1, 1,        //2 left_odd
    0, 1, 1, 1, 0, 0, 1,        //5 left_even
    0, 0, 1, 1, 1, 0, 1,        //4 left_even
    0, 1, 1, 1, 1, 0, 1,        //3 left_odd
    0, 0, 0, 1, 0, 0, 1,        //8 left_even
    0, 1, 0, 1, 0,              //center
    1, 0, 1, 1, 1, 0, 0,        //4 right
    1, 1, 0, 0, 1, 1, 0,        //1 right
    1, 0, 0, 1, 1, 1, 0,        //5 right
    1, 0, 1, 1, 1, 0, 0,        //4 right
    1, 1, 0, 0, 1, 1, 0,        //1 right
    1, 1, 1, 0, 1, 0, 0,        //9 right
    1, 0, 1,                    //trailer
    0, 0, 0, 0, 0, 0, 0, 0, 0   //quiet
};

static void s_print_bool_arr(bool* arr, int n)
{
    int i;

    for (i = 0; i < n; i++) {
        printf("%d", (int)arr[i]);
    }
    printf("\r\n");
}

int main(int argc, char** argv)
{

    int ean_size = EAN13_width();
    printf("EAN barcode size: %d\r\n", ean_size);

    bool code[ean_size];

    EAN13_build(test, code);

    for (int i = 0; i < ean_size; i++) {
        if (code[i] != test_code[i]) {
            printf("Error in barcode at position: %d\r\n", i);
            break;
        }
    }

    printf("Created:  ");
    s_print_bool_arr(code, ean_size);
    printf("Expected: ");
    s_print_bool_arr(test_code, ean_size);

    return 0;
}