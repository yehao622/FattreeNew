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
//        core[num_core]: Switch {
//            @display("p=305,99,r,20;is=s");
//        }
//
//        aggr[num_aggr]: Switch {
//            @display("p=207.50499,149.00624,r,20;is=s");
//        }
//
//        edge[num_edge]: Switch {
//            @display("p=152.31749,204.19374,r,20;is=s");
//        }

//        cn[num_cn]: ComputeNode {
//            @display("p=152.31749,281.45624,m,16,20;is=s");
//        }

//        oss[num_oss]: OSS {
//            @display("p=740.6162,166.66624,r,20;is=s");
//        }

        //        mds: MDS {
        //            @display("p=1196.0062,115.912506");
        //        }
        sink[2]: Sink {
            @display("p=556.73126,667.375,r,20;is=s");
        }
        cn[num_cn]: ComputeNode {
            @display("p=110.64375,414.475,m,8,40;is=n");
        }
        edge[num_edge]: Buffer {
            @display("p=261.68124,179.1375,r,40;i=old/srouter");
        }
        aggr[num_aggr]: Buffer {
            @display("i=old/srouter,#26A269;p=261.68124,103.61875,r,40");
        }
        core[num_core]: Buffer {
            @display("p=261.68124,36.881252,r,80;i=old/srouter,#ED333B");
        }
        oss[num_oss]: OSS {
            @display("p=707.76874,481.21252,r,40");
        }
        edge_connect[num_edge]: Payload {
            @display("p=261.68124,282.75626,r,40");
        }
    connections allowunconnected:
        for i=0..(num_edge-1) {
            edge[i].port++ <--> edge_connect[i].port++;
            edge_connect[i].port++ <--> sink[0].port++;
            edge_connect[i].port++ <--> sink[1].port++;
        }
        for i=0..(int(2*num_edge/edge_aggr_port)-1), for j=0..(int(edge_aggr_port/2)-1), for k=0..(int(edge_aggr_port/2)-1) {
            edge[i*int(edge_aggr_port/2)+j].port++ <--> aggr[i*int(edge_aggr_port/2)+k].port++;
        }
        for i=0..(num_core-1), for j=0..(core_port-1) {
            core[i].port++ <--> aggr[(i*core_port + j) % num_aggr].port++;
        }

        for i=0..(num_edge-1), for j=0..(int(edge_aggr_port/2)-2) {
            cn[int(i*(edge_aggr_port/2-1)) + j].out --> edge_connect[i].in++;
            edge_connect[i].out++ --> cn[int(i*(edge_aggr_port/2-1)) + j].in;
        }

        for i=0..(num_oss-1) {
            oss[i].out --> edge_connect[i].in++;
            edge_connect[i].out++ --> oss[i].in;
        }

//        for i=0..(num_aggr-1), for j=0..(int(K_port/2-1)) {
//            core[(i * int(K_port/2)) % num_core + j].port++ <--> Link <--> aggr[i].port++;
//        }

//        for i=0..(num_oss-1), for j=0..(int(K_port/2-1)) {
//            core[(i * int(K_port/2)) % num_core + j].port++ <--> Link <--> oss[i].port++;
//        }

//        for i=0..(num_core-1) {
//            core[i].port++ <--> mds.port++;
//        }

//        for i=0..(pod_cn-1), for j=0..(int(K_port/2-1)), for k=0..(int(K_port/2-1)) {
//            aggr[i * int(K_port/2) + j].port++ <--> Link <--> edge[i * int(K_port/2) + k].port++;
//        }

//        for i=0..(num_edge-1), for j=0..(int(K_port/2-1)) {
//            edge[i].port++ <--> Link <--> cn[i * int(K_port/2) + j].port++;
//        }

//        for i=0..(num_ost-1) {
//            ost[i].port++ <--> ned.IdealChannel <--> oss[i % num_oss].port++;
//        }

}
