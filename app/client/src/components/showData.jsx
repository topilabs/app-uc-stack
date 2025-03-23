import React from 'react';
// import { Buffer } from 'node:buffer';

export function ShowData({ data }) {

  // convert data to a buffer
  // buffer = Buffer.from(data);
  console.log("rx data:", data);
  // console.log("decoded data:", cobs.decode(data));

  return (
    // convert arraybuffer, called data, to a string displaying the hex values in html
    <div>
      <h3> Raw data: </h3>
      <h3> { Array.prototype.map.call(new Uint8Array(data), x => ('00' + x.toString(16)).slice(-2)).join(' ') } </h3>
      <h3> Decoded data: </h3>

    </div>
 
  );
}