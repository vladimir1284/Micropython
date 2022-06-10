<script lang="ts">
	import { onMount } from 'svelte';

    export let id: string; 
	export let level: number;
	export let capacity: number;
	export let min_level: number;

	$: {
		try {
			updateTankValue(level);
		} finally {
		}
	}

	let display = '';

	let ctx: CanvasRenderingContext2D;
	let yc: number;
	let xc: number;
	let h: number;
	let w: number;

	function init() {
		let canvas = document.getElementById('tank'+id) as HTMLCanvasElement;
		if (canvas.getContext) {
            w = 0.8 *canvas.width;
            h = w;

            if (h > (0.8 * canvas.height)) {
                h = 0.8 * canvas.height;
                w = h;
            }

			xc = 0.5 * canvas.width;
			yc = 0.5 * canvas.height;

			ctx = canvas.getContext('2d') as CanvasRenderingContext2D;

			ctx.fillStyle = '#000000';
			ctx.strokeRect(xc - w / 2, 0.2 * h, w, h);
			ctx.fillRect(xc - w / 4, 0.1 * h, w / 2, 0.1 * h);
		}
	}

	function updateTankValue(value: number) {
		if (ctx != undefined) {
			if (value >= 0 && value <= 100) {
				display = (value * capacity) / 100 + ' litros';

				if (value > min_level) {
					ctx.fillStyle = '#0000FF';
				} else {
					ctx.fillStyle = '#FF0000';
				}

				ctx.clearRect(xc - w / 2 + 1, 0.2 * h + 1, w - 2, h - 2);
				ctx.fillRect(
					xc - w / 2 + 1,
					(1 - value / 100) * (h - 2) + 0.2 * h + 1,
					w - 2,
					(value / 100) * (h - 2)
				);
			} else {
				display = '¡Valor incorrecto!';
			}
		}
	}

	// Do the job once the DOM was generated
	onMount(init);
</script>

<div class="container">
	<canvas id={'tank'+id} />
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
