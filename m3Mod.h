/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   m3Mod.h
 * Author: zhan2860
 *
 * Created on March 19, 2017, 7:09 PM
 */

#ifndef M3MOD_H
#define M3MOD_H

vector<unsigned> find_POI_ids_from_name(std::string POI_name);
unsigned find_path_POI_helper (unsigned srcID, string POI_Name, double turn_penalty);
void resetNodes();
#endif /* M3MOD_H */

