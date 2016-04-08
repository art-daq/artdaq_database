#!/usr/bin/env node
var libname=process.argv.slice(0)[1].replace( /\\/g, '/' ).replace( /\/[^\/]*$/, '' ) + "/../lib/node_modules/fhicljson/fhicljson.node"

var mylib = require(libname)

var inp=process.argv.slice(1)[1].toString()
var out=inp + ".conv"

var result = mylib.tojson(inp)

if(result.first==true) {

    var json = JSON.parse(result.second)

    var result =  mylib.tofhicl(out,result.second)

    if(result.first!=true)
      console.log(result.second)  
    else 
      console.log("Wrote file " + out)  
    } 

else {
  console.log(result.second)  
}
