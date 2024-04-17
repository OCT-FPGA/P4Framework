<project xmlns="com.autoesl.autopilot.project" top="saveSrcDest_kernel" name="saveSrcDestV2" projectType="C/C++">
    <includePaths/>
    <libraryPaths/>
    <Simulation>
        <SimFlow name="csim"/>
    </Simulation>
    <files xmlns="">
        <file name="src/saveSrcDest.hpp" sc="0" tb="false" cflags="" csimflags="" blackbox="false"/>
        <file name="src/saveSrcDest_hls_wrapper.cpp" sc="0" tb="false" cflags="" csimflags="" blackbox="false"/>
        <file name="src/saveSrcDest_kernel.cpp" sc="0" tb="false" cflags="" csimflags="" blackbox="false"/>
        <file name="../../test/test1.cpp" sc="0" tb="1" cflags="-Wno-unknown-pragmas" csimflags="" blackbox="false"/>
    </files>
    <solutions xmlns="">
        <solution name="solution1" status="active"/>
    </solutions>
</project>

