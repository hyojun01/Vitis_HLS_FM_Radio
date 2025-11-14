open_project -reset low_pass_filter_second
set_top low_pass_filter_second
add_file low_pass_filter_seond.cpp
open_solution solution1 -reset
set_part {xc7z010clg225-1}
create_clock -period 10ns
csynth_design
export_design -flow impl -format ip_catalog
