<project xmlns="com.autoesl.autopilot.project" name="saveSrcDestV3" top="saveSrcDest_kernel">
    <includePaths/>
    <libraryPaths/>
    <Simulation>
        <SimFlow name="csim" clean="true" csimMode="0" lastCsimMode="0"/>
    </Simulation>
    <files xmlns="">
        <file name="../test/test1.cpp" sc="0" tb="1" cflags=" -I../include  -Wno-unknown-pragmas" csimflags=" -Wno-unknown-pragmas" blackbox="false"/>
        <file name="saveSrcDestV3/src/saveSrcDest_kernel.cpp" sc="0" tb="false" cflags="" csimflags="" blackbox="false"/>
        <file name="saveSrcDestV3/src/saveSrcDest_hls_wrapper.cpp" sc="0" tb="false" cflags="" csimflags="" blackbox="false"/>
        <file name="saveSrcDestV3/src/saveSrcDest.hpp" sc="0" tb="false" cflags="" csimflags="" blackbox="false"/>
        <file name="saveSrcDestV3/include/addr_types.h" sc="0" tb="false" cflags="" csimflags="" blackbox="false"/>
    </files>
    <solutions xmlns="">
        <solution name="solution1" status="active"/>
    </solutions>
</project>

