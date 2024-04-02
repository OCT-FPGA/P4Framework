# Calculator P4 Test
The `src` foler contains the input packets as .user format and corresponding table to be loaded, which has also been hard coded into the C-driver. 

IMPORTANT: MAKE SURE THE PATH for VIVADO and HLS in `run_behav.bash` are CORRECT.

The provided script will run a behavioral tests and then generated the expected output packets as in .pcap format in the `gen` foler alongside with an input packets as in pcap which can be used for end-to-end test.


After running the `./run_behav.bash`, use the `tcpdump -r gen/behav_reverse_tuple_in.pcap` and `tcpdump -r gen/behav_reverse_tuple_out.pcap` to check the tuples for each packet is swapped.
