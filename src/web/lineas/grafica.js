new Morris.Line({
  // ID of the element in which to draw the chart.
  element: 'myfirstchart',
  // Chart data records -- each entry in this array corresponds to a point on
  // the chart.
  data: [
    {time: 0,Temp1:+686.91,Temp2:+688.42,Temp3:+680.52,Temp4:+588.89,Temp5:+20.000,Temp6:+28.000,Temp7:+50.000,Temp8:+38.000},
    {time: 5,Temp1:+686.91,Temp2:+688.42,Temp3:+680.52,Temp4:+588.89,Temp5:+20.000,Temp6:+28.000,Temp7:+50.000,Temp8:+38.000},
    {time: 10,Temp1:+686.91,Temp2:+688.42,Temp3:+680.52,Temp4:+588.89,Temp5:+20.000,Temp6:+28.000,Temp7:+50.000,Temp8:+38.000},
    {time: 15,Temp1:+686.91,Temp2:+688.42,Temp3:+680.52,Temp4:+588.89,Temp5:+20.000,Temp6:+28.000,Temp7:+50.000,Temp8:+38.000},
    {time: 20,Temp1:+686.91,Temp2:+688.42,Temp3:+680.52,Temp4:+588.89,Temp5:+20.000,Temp6:+28.000,Temp7:+50.000,Temp8:+38.000},
    {time: 25,Temp1:+686.91,Temp2:+688.42,Temp3:+680.52,Temp4:+588.89,Temp5:+20.000,Temp6:+28.000,Temp7:+50.000,Temp8:+38.000},
    {time: 30,Temp1:+686.91,Temp2:+688.42,Temp3:+680.52,Temp4:+588.89,Temp5:+20.000,Temp6:+28.000,Temp7:+50.000,Temp8:+38.000},
    {time: 35,Temp1:+686.91,Temp2:+688.42,Temp3:+680.52,Temp4:+588.89,Temp5:+20.000,Temp6:+28.000,Temp7:+50.000,Temp8:+38.000},
  ],
  // The name of the data record attribute that contains x-values.
  xkey: 'time',
  // A list of names of data record attributes that contain y-values.
  //Los 8 canales irán en las y.
  ykeys: ['Temp1','Temp2','Temp3','Temp4','Temp5','Temp6','Temp7','Temp8'],
  // Labels for the ykeys -- will be displayed when you hover over the
  // chart.
  labels: ['Temp1','Temp2','Temp3','Temp4','Temp5','Temp6','Temp7','Temp8'],
  resize:true,
  smooth: false
});
