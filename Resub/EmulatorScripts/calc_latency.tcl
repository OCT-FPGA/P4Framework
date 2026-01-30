# Usage:
# vivado -mode batch -source lat.tcl -tclargs /abs/path/to/file.p4 300 512

set p4_file   [file normalize [lindex $argv 0]]
set pkt_rate  [lindex $argv 1]          ;# Mpps
set bus_width [lindex $argv 2]          ;# bits (e.g., 512)

set tdata_num_bytes [expr {$bus_width / 8}]

create_project -in_memory -part xcu280-fsvh2892-2L-e

create_ip -name vitis_net_p4 -vendor xilinx.com -library ip -module_name vitis_net_p4_0
set ip [get_ips vitis_net_p4_0]

set_property CONFIG.P4_FILE         $p4_file         $ip
set_property CONFIG.PKT_RATE        $pkt_rate        $ip
set_property CONFIG.TDATA_NUM_BYTES $tdata_num_bytes $ip

generate_target all $ip

set_property CONFIG.UPDATE_TOTAL_LATENCY FALSE $ip
set_property CONFIG.UPDATE_TOTAL_LATENCY TRUE  $ip
generate_target all $ip

puts [get_property CONFIG.TOTAL_LATENCY $ip]
exit 0
