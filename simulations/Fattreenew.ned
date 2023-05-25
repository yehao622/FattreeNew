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
        @display("bgb=1103.04,908.476");
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
            @display("p=689.4,418.23602,r,20;is=s");
        }
        cn[num_cn]: ComputeNode {
            @display("p=73.536,640.37604,m,8,40;is=n");
        }
        edge[num_edge]: Buffer {
            @display("p=260.44,340.104,r,40;i=old/srouter");
        }
        aggr[num_aggr]: Buffer {
            @display("i=old/srouter,#26A269;p=260.44,177.712,r,40");
        }
        core[num_core]: Buffer {
            @display("p=261.68124,36.881252,r,80;i=old/srouter,#ED333B");
        }
        oss[num_oss]: OSS {
            @display("p=709.31604,603.60803,r,40");
        }
        edge_connect[num_edge]: Payload {
            @display("p=260.44,430.492,r,40");
        }
        inif_edge_cn[int(edge_aggr_port*edge_aggr_port/2)]: Infiniband {
            @display("p=260.44,531.604");
        }
        inif_aggr_edge[int(num_edge * edge_aggr_port/2)]: Infiniband {
            @display("p=105.708,265.036,r,20");
        }
        inif_core_aggr[num_core * core_port]: Infiniband {
            @display("p=105.708,90.388,r,20");
        }
    connections allowunconnected:
        for i=0..(num_edge-1) {
            edge[i].port++ <--> edge_connect[i].port++;
            edge_connect[i].port++ <--> sink[0].port++;
            edge_connect[i].port++ <--> sink[1].port++;
        }
        for i=0..(int(num_edge * edge_aggr_port/2) - 1) {
            edge[int(i/(edge_aggr_port/2))].port++ <--> inif_aggr_edge[i].port++;
        }
        for i=0..(num_core-1), for j=0..(int(edge_aggr_port/2) * int(edge_aggr_port/2) - 1) {
            inif_aggr_edge[i * int(edge_aggr_port/2) * int(edge_aggr_port/2) + j].port++ <--> aggr[i * int(edge_aggr_port/2) + j % int(edge_aggr_port/2)].port++;
        }
        for i=0..(int(num_aggr * edge_aggr_port/2) - 1) {
            aggr[int(i/(edge_aggr_port/2))].port++ <--> inif_core_aggr[i].port++;
            inif_core_aggr[i].port++ <--> core[i % num_core].port++;
        }

        for i=0..(num_cn-1) {
            cn[i].port++ <--> inif_edge_cn[i].port++;
            inif_edge_cn[i].port++ <--> edge_connect[int(i/(int(edge_aggr_port/2)-1))].port++;
        }
        for i=0..(num_oss-1) {
            oss[i].port++ <--> inif_edge_cn[i+num_cn].port++;
            inif_edge_cn[i+num_cn].port++ <--> edge_connect[i].port++;
        }

}
