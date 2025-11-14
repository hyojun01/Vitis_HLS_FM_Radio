open_project -reset fir_decimation_filter
set_top fir_decimation_filter
add_file fir_decimation_filter.cpp
open_solution solution1 -reset
set_part {xc7z010clg225-1}
create_clock -period 10ns
csynth_design
export_design -flow impl -format ip_catalog
