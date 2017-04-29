function DrawArea(width, height, margins) {
  function Margins(top, bottom, left, right) {
    this.top = top,
    this.bottom = bottom,
    this.left = left,
    this.right = right;
  }

  this._buildAxisReferences = function(width, height, margins) {
    this.width = width - (margins.left + margins.right);
    this.height = height - (margins.top + margins.bottom);
    this.margins = margins;

    this.x = new Object();
    this.x.left  = this.margins.left;
    this.x.right = this.margins.left + this.width;
    this.x.center = this.x.left + (this.width / 2);

    this.y = new Object();
    this.y.top = this.margins.top;
    this.y.bottom = this.margins.top + this.height;
    this.y.center = this.y.top + (this.height / 2);
  }

  this.fromDrawAreaUsingMargin = function(drawArea, margins) {
    margins = new Margins(margins[0], margins[1], margins[2], margins[3]);
    this._buildAxisReferences(drawArea.width, drawArea.height, margins);

    return this;
  }

  if(margins != undefined)
    margins = new Margins(margins[0], margins[1], margins[2], margins[3]);
  else
    margins = new Margins(0, 0, 0, 0);

  if(width == undefined && height == undefined)
    width = height = 0;

  this._buildAxisReferences(width, height, margins);
}

export default function BuildGraph() {

  var divwith = d3.select("#grapharea").node().getBoundingClientRect().width;

  /* Set the dimensions and margins of the grid */
  var drawArea = new DrawArea(divwith, 500);
  var gridArea = new DrawArea().fromDrawAreaUsingMargin(drawArea, [30, 50, 50, 30]);

  var fakeData = [4076,4082,4073,4027,3979,3921,3852,3781,3700,3616,3525,3429,3329,3218,3109,2998,2878,2756,2632,2508,2382,2252,2122,1995,1868,1740,1615,1493,1371,1258,1144,1034,931,839,752,673,599,534,473,425,380,341,311,280,258,237,219,204,191,182,178,170,170,200,246,305,371,444,525,610,703,800,901,1009,1117,1237,1357,1477,1602,1730,1856,1986,2116,2244,2374,2506,2631,2758,2880,2997,3113,3227,3333,3435,3528,3613,3690,3755,3809,3861,3895,3931,3957,3983,4003,4019,4032,4047,4059,4064,4075,4087,4079,4031,3979,3924,3860,3791,3711,3625,3535,3438,3340,3231,3121,3005,2886,2767,2645,2515,2390,2264,2135,2008,1878,1750,1627,1506,1388,1268,1156,1048,947,851,766,682,610,544,486,433,387,348,315,286,260,241,222,208,190,183,181,172,170,194,246,306,375,444,522,612,706,800,901,1013,1119,1237,1357,1480,1605,1728,1856,1984,2114,2244,2371,2495,2619,2743,2859,2978,3090,3201,3311,3408,3503,3632,3704,3767,3820,3864,3905,3933,3961,3984,4006,4019,4037,4051,4058,4071];


  /* Set the ranges */
  var x = d3.scaleLinear().range([0, gridArea.width]);
  var y = d3.scaleLinear().range([gridArea.height, 0]);

  // define the line
  var valueline = d3.line()
      .x(function(d) { return x(d.x); })
      .y(function(d) { return y(d.y); });

  // append the svg obgect to the body of the page
  // appends a 'group' element to 'svg'
  // moves the 'group' element to the top left margin
  var svg = d3.select("#grapharea").append("svg")
      .attr("class", "graph")
      .attr("width", drawArea.width)
      .attr("height", drawArea.height);

  var graphArea = svg.append("g")
                  .attr("transform",
                  "translate(" + gridArea.x.left + "," + gridArea.y.top + ")");

  var l = d3.line()
              .x(function(d){ return d[0];})
              .y(function(d){ return d[1];});

       // .call(d3.axisLeft(x)
        //           .tickSize());

  function make_x_gridlines() { 
    return d3.axisBottom(x)
          .ticks(20)
  }
  // gridlines in y axis function
  function make_y_gridlines() { 
    return d3.axisLeft(y)
          .ticks(10)
  }

    graphArea.append("rect")
        .attr("x", "0")
        .attr("y", "0")
        .attr("width", gridArea.width)
        .attr("height", gridArea.height)
        .attr("class", "grid-area");


    graphArea.append("g")
        .attr("class", "grid")
        .attr("transform", "translate(0, " + (gridArea.height) + ")")
        .call(make_x_gridlines()
            .tickSize(-gridArea.height)
            .tickFormat("")
        );

    graphArea.append("g")
        .attr("class", "grid")
        .attr("transform", "translate(" + 0 + ", 0)")
        .call(make_y_gridlines()
            .tickSize(-gridArea.width, 0, 0)
            .tickFormat("")
        );


  // Get the data
  if(1) {
    data = []
    for(var i = 0; i < 100; i++) {
      data.push({x: i, y:i*10});
    }
    // Scale the range of the data
    x.domain([0, 200]);
    y.domain([-5000, 5000]);

  /* Left-vertical grid side */
  svg.append("path")
        .data([[[gridArea.x.left, gridArea.y.top], [gridArea.x.left, gridArea.y.bottom]]])
        .attr("class", "center-grid")
        .attr("d", l);

  /* Center-vertical grid side */
  svg.append("path")
        .data([[[gridArea.x.center, gridArea.y.top], [gridArea.x.center, gridArea.y.bottom]]])
        .attr("class", "center-grid")
        .attr("d", l);

  /* Right-vertical grid side */
  svg.append("path")
        .data([[[gridArea.x.right, gridArea.y.top], [gridArea.x.right, gridArea.y.bottom]]])
        .attr("class", "center-grid")
        .attr("d", l);

  /* Center-horizontal grid side */
  svg.append("path")
        .data([[[gridArea.x.left, gridArea.y.center], [gridArea.x.right, gridArea.y.center]]])
        .attr("class", "center-grid")
        .attr("d", l);

  /* Top-horizontal grid side */
  svg.append("path")
        .data([[[gridArea.x.left, gridArea.y.top], [gridArea.x.right, gridArea.y.top]]])
        .attr("class", "center-grid")
        .attr("d", l);

  /* Bottom-horizontal grid side */
  svg.append("path")
        .data([[[gridArea.x.left, gridArea.y.bottom], [gridArea.x.right, gridArea.y.bottom]]])
        .attr("class", "center-grid")
        .attr("d", l);

    // Add the X Axis
    var boh = graphArea.append("g")
        .attr("transform", "translate(0," + (gridArea.height+10) + ")")
        .attr("class", "axisBottom")
        .call(d3.axisBottom(x)
                .tickSize(0));

    // Add the Y Axis
    graphArea.append("g")
        .attr("transform", "translate(-5," + (0) + ")")
        .attr("class", "axisBottom")
        .call(d3.axisLeft(y)
              .tickSize(0)
              // .tickValues(d3.range(-5, 6, 1))
        );

    var freqencyArea = graphArea.append("g")
        .attr("transform", "translate(" + (gridArea.x.right -220) + ", " + (gridArea.y.bottom - 80) + ")")
        .attr("class", "frequency-rect");

        freqencyArea.append("rect")
        .attr("width", 150)
        .attr("height", 20)
        .attr("rx", 10);

        freqencyArea.append("text")
        .attr("x", 10)
        .attr("y", 20 / 2)
        .attr("dy", ".35em")
        .attr("fill", "black")
        .text("f⎓100MHz");

  dataset = [];

  for (var i = 0; i < 200; i++)
    dataset.push({x: i, y: fakeData[i]});

  valueline = d3.line()
      .x(function(d) { return x(d.x); })
      .y(function(d) { return y(d.y); });

    graphArea.append("path")
        .data([dataset])
        .attr("class", "line")
        .attr("d", valueline)
}