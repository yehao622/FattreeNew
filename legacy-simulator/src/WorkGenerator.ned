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
simple WorkGenerator
{
    parameters:
        @display("i=old/gen");
        bool sendInitialMessage = default(false);
        int rng = default(0);
        double data_size = default(1.0);
        double read_probability = default(0.5);
        double cn_probability = default(0.0);
        double sendInterval @unit(s) = default(1e-3s);
    gates:
        inout port;
}
