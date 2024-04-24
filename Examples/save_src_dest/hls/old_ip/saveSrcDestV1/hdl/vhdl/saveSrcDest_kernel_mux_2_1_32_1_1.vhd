-- ==============================================================
-- Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2023.1 (64-bit)
-- Tool Version Limit: 2023.05
-- Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
-- Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
-- 
-- ==============================================================

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;

entity saveSrcDest_kernel_mux_2_1_32_1_1 is
generic (
    ID            :integer := 0;
    NUM_STAGE     :integer := 1;
    din0_WIDTH       :integer := 32;
    din1_WIDTH       :integer := 32;
    din2_WIDTH       :integer := 32;
    dout_WIDTH        :integer := 32);
port (
    din0   :in  std_logic_vector(31 downto 0);
    din1   :in  std_logic_vector(31 downto 0);
    din2   :in  std_logic_vector(0 downto 0);
    dout     :out std_logic_vector(31 downto 0));
end entity;

architecture rtl of saveSrcDest_kernel_mux_2_1_32_1_1 is
    -- puts internal signals
    signal sel    : std_logic_vector(0 downto 0);
    -- level 1 signals
    signal mux_1_0    : std_logic_vector(31 downto 0);
begin

sel <= din2;

-- Generate level 1 logic
mux_1_0 <= din0 when sel(0) = '0' else din1;

-- output logic
dout <= mux_1_0;

end architecture;
