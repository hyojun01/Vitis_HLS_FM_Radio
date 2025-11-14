open_project -reset low_pass_filter_first
set_top low_pass_filter_first
add_file low_pass_filter_first.cpp
open_solution solution1 -reset
set_part {xc7z010clg225-1}
create_clock -period 10ns
csynth_design
export_design -flow impl -format ip_catalog
