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

package fattreenew.simulations;

import fattreenew.Buffer;
import fattreenew.Payload;
import fattreenew.Infiniband;
import fattreenew.SAS;
import fattreenew.PCIe;
import fattreenew.ComputeNode;
import fattreenew.OST;
import fattreenew.Sink;
import fattreenew.HCA;
import fattreenew.OSS;
import ned.DatarateChannel;
import ned.DelayChannel;

channel Link extends ned.DatarateChannel
{
    datarate = 40Gbps;
    delay = 0.1us;
    @display("ls=#1A5FB4");
}

simple dramBuffer extends Buffer
{
    @display("i=,#62A0EA");
}

simple sramBuffer extends Buffer
{
    @display("i=,#F8E45C");
}

//
// Two instances (tic and toc) of Txc connected.
//
network Fattreenew
{
    parameters:
        @display("bgb=960.66876,762.2125");
        int core_port = default(16); // each switch has 4 ports, make sure 'num_core_oss' strictly less than this variable!
        int edge_aggr_port = default(8);
        //int pod_oss = default(3); //EACH core switch spares 'num_core_oss' links to OSSes, then (num_core_oss * core.port_num) links availbale to OSSes
        //int pod_cn = K_port - pod_oss - 1; // 1 MDS here 

        int num_core = default(2);//int(K_port * K_port / 4);2
        int num_aggr = default(8);//int(K_port/2 * pod_cn);8
        int num_edge = default(8);//int(K_port/2 * pod_cn);8
        int num_oss = default(8); // Can NOT greater than 'pod_oss * K_port/2' 8
        int num_cn = int(edge_aggr_port/2 * edge_aggr_port - num_oss);

    submodules:
        sink[2]: Sink {
            @display("p=560.71204,713.91205,r,20;is=s");
        }
        cn[num_cn]: ComputeNode {
            @display("p=111.836006,470.324,m,8,40;is=n");
        }
        edge[num_edge]: Buffer {
            @display("p=260.44,240.524,r,40;i=old/srouter");
        }
        aggr[num_aggr]: Buffer {
            @display("i=old/srouter,#26A269;p=260.44,131.752,r,40");
        }
        core[num_core]: Buffer {
            @display("p=261.68124,36.881252,r,80;i=old/srouter,#ED333B");
        }
        oss[num_oss]: OSS {
            @display("p=709.31604,603.60803,r,40");
        }
        edge_connect[num_edge]: Payload {
            @display("p=260.44,361.552,r,40");
        }
        inif_edge_cn[num_edge]: Infiniband {
            @display("p=260.44,300.272,r,40");
        }
        inif_aggr_edge[int(num_edge * edge_aggr_port/2)]: Infiniband {
            @display("p=144.00801,185.37201,r,20");
        }
        inif_core_aggr[num_core * core_port]: Infiniband {
            @display("p=144.00801,85.792,r,20");
        }
    connections allowunconnected:
        for i=0..(num_edge-1) {
            edge[i].port++ <--> inif_edge_cn[i].port++;
            inif_edge_cn[i].port++ <--> edge_connect[i].port++;
            edge_connect[i].port++ <--> sink[0].port++;
            edge_connect[i].port++ <--> sink[1].port++;
        }
        for i=0..(int(num_edge * edge_aggr_port/2) - 1) {
            edge[int(i/(edge_aggr_port/2))].port++ <--> inif_aggr_edge[i].port++;
        }
        for i=0..(num_core-1), for j=0..(int(edge_aggr_port/2) * int(edge_aggr_port/2) - 1) {
            inif_aggr_edge[i * int(edge_aggr_port/2) * int(edge_aggr_port/2) + j].port++ <--> aggr[i * int(edge_aggr_port/2) + j % int(edge_aggr_port/2)].port++;
        }
//        for i=0..(num_core-1), for j=0..(core_port-1) {
//            core[i].port++ <--> aggr[(i*core_port + j) % num_aggr].port++;
//        }
        for i=0..(int(num_aggr * edge_aggr_port/2) - 1) {
            aggr[int(i/(edge_aggr_port/2))].port++ <--> inif_core_aggr[i].port++;
            inif_core_aggr[i].port++ <--> core[i % num_core].port++;
        }

        for i=0..(num_edge-1), for j=0..(int(edge_aggr_port/2)-2) {
            cn[int(i*(edge_aggr_port/2-1)) + j].out --> edge_connect[i].in++;
            edge_connect[i].out++ --> cn[int(i*(edge_aggr_port/2-1)) + j].in;
        }

        for i=0..(num_oss-1) {
            oss[i].out --> edge_connect[i].in++;
            edge_connect[i].out++ --> oss[i].in;
        }

}
