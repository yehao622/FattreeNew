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
simple StorageDevice
{
    parameters:
        @display("i=device/disk");
        int parallel_level = default(2);
        int max_queue_len = default(512);
        double read_bw @unit(Mbps) = default(1600Mbps);
        double write_bw @unit(Mbps) = default(800Mbps);  // 100~200 MB/s for HDD; 500~3072 MB/s for flash memory   
        
        @signal[queueLength](type="int");
        @statistic[queueLength](title="Queue length"; record=stats,vector);
    gates:
        inout port[];
}
