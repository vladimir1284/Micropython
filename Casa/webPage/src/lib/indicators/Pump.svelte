<script lang="ts">
	import { onMount } from 'svelte';

	const STATES: Record<string, string> = {
		IDLE: 'Apagada',
		PUMPING: 'Encendida',
		WAIT: 'Esperando',
		ERROR: 'Error'
	};
	const CMAP: Record<string, string> = {
		WAIT: "#fafd08",
		ERROR: "#ff0000"
	};

	export let state: string;
	export let pumping: boolean;

	$: updatePumpValue(state, pumping);

	let display = 'Apagada';
	let animationStage = 0;
	let animate: NodeJS.Timer;
    let backgroung_color = "transparent";

	let ctx: CanvasRenderingContext2D;
	let yc: number;
	let xc: number;
	let h: number;
	let w: number;
	let x0: number;
	let y0: number;
	let w0: number;
	let pxl: number;
	let dy2: number;
	let dy1: number;

	function init() {
		let canvas = document.getElementById('pump') as HTMLCanvasElement;
		if (canvas.getContext) {
			w = 0.8 * canvas.width;
			h = w;

			if (h > 0.8 * canvas.height) {
				h = 0.8 * canvas.height;
				w = h;
			}

			xc = 0.5 * canvas.width;
			yc = 0.5 * canvas.height;

			pxl = w / 48;
			x0 = xc - w / 2;
			y0 = 0.2 * h;
			w0 = (3 * w) / 40;
			dy2 = (20 * h) / 40;
			dy1 = (10 * h) / 40;

			ctx = canvas.getContext('2d') as CanvasRenderingContext2D;

			ctx.beginPath();
			ctx.translate(x0, y0);
			ctx.moveTo(5 * pxl, 0);
			ctx.lineTo(8 * pxl, 0);
			ctx.lineTo(8 * pxl, 14 * pxl);
			ctx.lineTo(5 * pxl, 15 * pxl);
			ctx.lineTo(5 * pxl, 30 * pxl);
			ctx.lineTo(8 * pxl, 31 * pxl);
			ctx.lineTo(13 * pxl, 31 * pxl);
			ctx.moveTo(16 * pxl, 0);
			ctx.lineTo(13 * pxl, 0);
			ctx.lineTo(13 * pxl, 0);
			ctx.lineTo(13 * pxl, 34 * pxl);
			ctx.lineTo(16 * pxl, 34 * pxl);
			ctx.lineTo(21 * pxl, 28 * pxl);
			ctx.lineTo(24 * pxl, 28 * pxl);
			ctx.lineTo(25 * pxl, 32 * pxl);
			ctx.lineTo(46 * pxl, 32 * pxl);
			ctx.lineTo(47 * pxl, 31 * pxl);
			ctx.lineTo(47 * pxl, 15 * pxl);
			ctx.lineTo(46 * pxl, 14 * pxl);
			ctx.lineTo(25 * pxl, 14 * pxl);
			ctx.lineTo(24 * pxl, 18 * pxl);
			ctx.lineTo(21 * pxl, 18 * pxl);
			ctx.lineTo(16 * pxl, 12 * pxl);
			ctx.lineTo(13 * pxl, 12 * pxl);
			ctx.moveTo(29 * pxl, 14 * pxl);
			ctx.lineTo(29 * pxl, 28 * pxl);
			ctx.moveTo(32 * pxl, 27 * pxl);
			ctx.lineTo(41 * pxl, 27 * pxl);
			ctx.moveTo(32 * pxl, 23 * pxl);
			ctx.lineTo(41 * pxl, 23 * pxl);
			ctx.moveTo(32 * pxl, 19 * pxl);
			ctx.lineTo(41 * pxl, 19 * pxl);
			ctx.moveTo(43 * pxl, 10 * pxl);
			ctx.lineTo(43 * pxl, 8 * pxl);
			ctx.lineTo(31 * pxl, 8 * pxl);
			ctx.lineTo(29 * pxl, 10 * pxl);
			ctx.moveTo(30 * pxl, 35 * pxl);
			ctx.lineTo(26 * pxl, 38 * pxl);
			ctx.lineTo(24 * pxl, 38 * pxl);
			ctx.lineTo(24 * pxl, 40 * pxl);
			ctx.lineTo(46 * pxl, 40 * pxl);
			ctx.lineTo(46 * pxl, 38 * pxl);
			ctx.lineTo(44 * pxl, 38 * pxl);
			ctx.lineTo(40 * pxl, 35 * pxl);
			ctx.strokeRect(0, 20 * pxl, 5 * pxl, 6 * pxl);
			ctx.stroke();
			ctx.translate(-x0, -y0);
		}
	}

	function updatePumpValue(state: string, pumping: boolean) {
		if (ctx != undefined) {
			display = STATES[state];

            // Alert on error
            ctx.translate(x0, y0);
            ctx.lineWidth = 3;
            if(['ERROR', 'WAIT'].indexOf(state) >= 0){
                const color = CMAP[state];
                ctx.strokeStyle = color;
                ctx.strokeRect(-2*pxl, -5 * pxl, 53 * pxl, 50 * pxl);
                ctx.strokeStyle = "#000000";
            } else {
                ctx.clearRect(-3*pxl, -6 * pxl, 55 * pxl, 54 * pxl);
            }
            ctx.translate(-x0, -y0);
            ctx.lineWidth = 1;
            init();

			if (pumping) {
				animate = setInterval(animatePump, 500);
			} else {
				clearInterval(animate);
				const x0 = xc - 0.32 * w;
				ctx.clearRect(x0 - 1, yc - dy2, w0 + 1, (20 * h) / 40);
			}
		}
	}

	function animatePump() {
		ctx.fillStyle = '#0000FF';
		const x0 = xc - 0.32 * w;
		switch (animationStage) {
			case 0:
				ctx.clearRect(x0 - 1, yc - dy2, w0 + 1, (20 * h) / 40);
				animationStage = 1;
				break;
			case 1:
				ctx.fillRect(x0, yc - dy1, w0, (10 * h) / 40);
				animationStage = 2;
				break;
			case 2:
				ctx.fillRect(x0, yc - dy2, w0, (10 * h) / 40);
				animationStage = 0;
				break;
		}
	}

	// Do the job once the DOM was generated
	onMount(init);
</script>

<div class="container">
	<canvas id="pump" style="background-color: {backgroung_color};"/>
	<div class="status">{display}</div>
</div>

<style>
	.container {
		display: grid;
		grid-template-columns: auto;
	}
	.status {
		text-align: center;
	}
</style>
