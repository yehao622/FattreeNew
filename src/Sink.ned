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

simple Sink
{
    parameters:
        @display("i=abstract/db;is=n");
        @signal[throughput](type="double");
        @signal[readThroughput](type="double");
        @signal[writeThroughput](type="double");
        @statistic[throughput](title="All throughput"; record=stats,vector);
        @statistic[readThroughput](title="Throughput of read operation in the system"; record=stats,vector);
        @statistic[writeThroughput](title="Throughput of write operation in the system"; record=stats,vector);
    gates:
        inout port[];
}
