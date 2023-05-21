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

import fattreenew.simulations.sramBuffer;

network HBA
{
    @display("i=block/ifcard,#62A0EA");
    gates:
        input in;
        output out;
    submodules:
        hbaBuffer: sramBuffer {
            @display("p=217,94");
        }
        hba_payload: Payload {
            @display("p=98,94");
        }
    connections:
		in --> hba_payload.in++;
		hba_payload.port++ <--> hbaBuffer.port++;
		hba_payload.out++ --> out;
}
