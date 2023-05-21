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

channel sasFibre extends ned.DatarateChannel
{
    datarate = 10Gbps;
    delay = 10us;
    @display("ls=#F5C211");
}

network SAS
{
    parameters:
        @display("i=abstract/table2,#F5C211;bgb=696,278");
        int num_link = default(4);
    gates:
        input in;
        output out;
    submodules:
        in_flow: Payload {
            @display("p=58,132");
        }
        link_input[num_link]: Payload {
            @display("p=249,132");
        }
        link_output[num_link]: Payload {
            @display("p=399,132");
        }
        out_flow: Payload {
            @display("p=577,132");
        }
    connections:
        in --> in_flow.in++;
        for i=0..(num_link-1) {
            in_flow.out++ --> link_input[i].in++;
            link_input[i].out++ --> sasFibre --> link_output[i].in++;
            link_output[i].out++ --> out_flow.in++;
        }
        out_flow.out++ --> out;
}
