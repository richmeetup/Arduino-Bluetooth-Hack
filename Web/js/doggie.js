var columnMap = { max: 0, data: [
	{
		x: 128,
		y: 141,
		count: 0
	},{
		x: 128,
		y: 268,
		count: 0
	},{
		x: 128,
		y: 396,
		count: 0
	},{
		x: 128,
		y: 523,
		count: 0
	},{
		x: 128,
		y: 639,
		count: 0
	},{
		x: 128,
		y: 754,
		count: 0
	},{
		x: 128,
		y: 870,
		count: 0
	},{
		x: 128,
		y: 998,
		count: 0
	},{
		x: 128,
		y: 1126,
		count: 0,
	},{
		x: 128,
		y: 1254,
		count: 0
	}
]};

var resetCounts = function() {
	for (i = 0; i < columnMap.data.length; i++) {
		columnMap.data[i].count = 0;
	}
	columnMap.max = 0;
}

var implementMap = function(locations, dog) {
	//Add to the count for each column
	for(i = 0; i < locations.length; i++) {
		columnMap.data[locations[i]].count++;	
	}
	//Figure out what the max is among all columns
	for(j = 0; j < columnMap.data.length; j++) {
		if(columnMap.data[j].count > columnMap.max) {
			columnMap.max = columnMap.data[j].count;
		}
	}
	//Create a dog specific heatmap
	if(dog === 'doug'){
		var dougxx = h337.create({'element':document.getElementById('dougHeatmapArea'), 'radius':90, 'visible':true});
		dougxx.store.setDataSet(columnMap);
		resetCounts();
	} else {
		var lolaxx = h337.create({'element':document.getElementById('lolaHeatmapArea'), 'radius':90, 'visible':true});
		lolaxx.store.setDataSet(columnMap);
		resetCounts();
	}

	
};

var handleData = function(listFromDevice, dog) {
	var locations = listFromDevice.split(' ');
	implementMap(locations, dog);
};