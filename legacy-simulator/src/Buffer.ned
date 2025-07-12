//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package fattreenew;

//
// TODO auto-generated module
//
simple Buffer
{
    parameters:
        int rng = default(0);
        
        double flash_buffer @unit(MB) = default(128.0MB);
//        double read_access_flash_latency @unit(s) = default(5.0e-4s); // Here, assume its latency is determined by following disk/ssd latency
//        double write_access_flash_latency @unit(s) = default(1.0e-3s);
        double read_storage_flash_bw @unit(Mbps) = default(40960Mbps);
        double write_storage_flash_bw @unit(Mbps) = default(20480Mbps);
        
        double DRAM_buffer @unit(MB) = default(2048.0MB);
//        double read_access_DRAM_latency @unit(s) = default(3.0e-8s);
//        double write_access_DRAM_latency @unit(s) = default(6.0e-8s);
        double read_DRAM_buffer_bw @unit(Mbps) = default(327680Mbps);
        double write_DRAM_buffer_bw @unit(Mbps) = default(262144Mbps);
        
        double SRAM_buffer @unit(MB) = default(2.0MB);
//        double SRAM_latency @unit(s) = default(5.0e-9s);
        double read_SRAM_buffer_bw @unit(Mbps) = default(1500000Mbps);
        double write_SRAM_buffer_bw @unit(Mbps) = default(1000000Mbps);
        
        double switch_buffer @unit(MB) = default(16.0MB);
        double read_switch_bw @unit(Mbps) = default(409600Mbps);
        double write_switch_bw @unit(Mbps) = default(204800Mbps);
        
        @display("i=block/buffer");
        
        @signal[queueLength](type="int");
        @statistic[queueLength](title="Queue length"; record=stats,vector);
    gates:
        inout port[];
}
