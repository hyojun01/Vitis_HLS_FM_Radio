#include "ap_fixed.h"
#include "ap_int.h"
#include "hls_stream.h"
#include "ap_axi_sdata.h"

#define NUM_TAPS 25
#define DECIMATION_FACTOR 5

typedef ap_fixed<16, 8> coef_t;
typedef ap_fixed<16, 8> data_t;
typedef hls::axis<data_t, 0, 0, 0> axis_t;
typedef ap_uint<3> count_t;

void fir_decimation_filter(hls::stream<axis_t>& input, hls::stream<axis_t>& output) {
#pragma HLS INTERFACE mode = axis port = input
#pragma HLS INTERFACE mode = axis port = output
// #pragma HLS INTERFACE mode = s_axilite port = return bundle = control 
#pragma HLS INTERFACE mode = ap_ctrl_none port = return 
#pragma HLS PIPELINE II = 1

    const coef_t coefficient[DECIMATION_FACTOR][5] = {
        {0.0193289951, 0.0263706029, 0.0934108918, -0.0620798848, -0.0234956754 },
        {0.0996113346, 0.0498237899, 0.3025603220, -0.0751905724, -0.0303894120 },
        {-0.0138526849, 0.0000086279, 0.3999505120, 0.0000086279, -0.0138526849 },
        {-0.0303894120, -0.0751905724, 0.3025603220, 0.0498237899, 0.0996113346 },
        {-0.0234956754, -0.0620798848, 0.0934108918, 0.0263706029, 0.0193289951 }
    };
    #pragma HLS ARRAY_PARTITION variable = coefficient type = complete dim = 0

    static data_t shift_register[DECIMATION_FACTOR][5];
    #pragma HLS ARRAY_PARTITION variable = shift_register type = complete dim = 0

    static count_t count = 0;
    static data_t sum = 0;

    axis_t input_temp;
    axis_t output_temp;

    input.read(input_temp);

    shift_polyphase_filter_register_loop:
    for (int i = 4; i > 0; i--) {
    #pragma HLS UNROLL
        shift_register[DECIMATION_FACTOR - 1 - count][i] = shift_register[DECIMATION_FACTOR - 1 - count][i - 1];
    }
    shift_register[DECIMATION_FACTOR - 1 - count][0] = input_temp.data;

    data_t sum_each = 0;
    polyphase_filter_convolution_loop:
    for (int i = 0; i < 5; i++) {
    #pragma HLS UNROLL
        sum_each += shift_register[DECIMATION_FACTOR - 1 - count][i] * coefficient[DECIMATION_FACTOR - 1 - count][i];
    }
    sum += sum_each;

    if (count == DECIMATION_FACTOR - 1) {
        output_temp.data = sum;
        count = 0;
        sum = 0;
        output.write(output_temp);
    }
    else {
        count++;
    }
}