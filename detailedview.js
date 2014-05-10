function drawStroke(stroke, ctx)
{
	console.log(stroke);
	var tokens = stroke.match(/[a-zA-Z]|-?[0-9\.]+/g);
	var x = 0.0, y = 0.0;
	var curCommand = 'M';
	ctx.beginPath();
	for (var i = 0; i < tokens.length; i++) {
		var t1 = 0.0, t2 = 0.0, t3 = 0.0, t4 = 0.0, t5 = 0.0, t6 = 0.0;

		/* new command? */
		if (tokens[i][0].match(/[a-z]/i))
			curCommand = tokens[i][0];

		/* relative coordinates? */
		if (curCommand.match(/[a-z]/)) {
			t1 = t3 = t5 = x;
			t2 = t4 = t6 = y;
		}

		switch (curCommand.toUpperCase()) {
			case 'M':
				t1 += parseFloat(tokens[i + 1]);
				t2 += parseFloat(tokens[i + 2]);
				ctx.moveTo(t1, t2);
				x = t1; y = t2;
				i += 2;
				break;
			case 'C':
				t1 += parseFloat(tokens[i + 1]);
				t2 += parseFloat(tokens[i + 2]);
				t3 += parseFloat(tokens[i + 3]);
				t4 += parseFloat(tokens[i + 4]);
				t5 += parseFloat(tokens[i + 5]);
				t6 += parseFloat(tokens[i + 6]);
				ctx.bezierCurveTo(t1, t2, t3, t4, t5, t6);
				x = t5; y = t6;
				i += 6;
				break;
			default:
				console.log("unknown (repeat?)");
				break;
		}
	}
	ctx.stroke();
}

function showEntry(type, code)
{
	console.log("Oki!");
	var json = entryLoader.loadEntry(type, code);
	console.log(json);

	var character;
	if (json != "")
		character = eval('(' +  json + ')');
	else
		return;

	var kanji = document.getElementById("kanji");
	kanji.innerHTML = character.id;

	return;

	var meanings = document.getElementById("meanings");
	meanings.innerHTML = character.meanings.join(", ");

	var canvas = document.getElementById("strokeAnim");
	var ctx = canvas.getContext("2d");
	ctx.scale(canvas.width / 110.0, canvas.height / 110.0);

	ctx.clearRect(0, 0, 110, 110);

	var strokes = character.strokes;

	ctx.strokeStyle = "rgb(50, 50, 100)";
	console.log(strokes);
	for (var i = 0; i < strokes.length; i++)
		drawStroke(strokes[i], ctx);
}
