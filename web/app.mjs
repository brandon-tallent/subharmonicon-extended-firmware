// patchFirmware and SYSEX_BYTES are embedded alongside this UI when packaged.

const input = document.querySelector('#firmware');
const status = document.querySelector('#status');
const download = document.querySelector('#download');
let outputUrl;

input.addEventListener('change', async () => {
  const file = input.files[0];
  if (!file) return;
  input.disabled = true;
  download.hidden = true;
  download.removeAttribute('href');
  if (outputUrl) URL.revokeObjectURL(outputUrl);
  status.textContent = 'Checking firmware…';
  status.dataset.error = 'false';
  try {
    if (!globalThis.crypto?.subtle) throw new Error('Use a current browser, or open the hosted patcher over HTTPS.');
    if (file.size !== SYSEX_BYTES) {
      throw new Error('Unzip Moog’s download and choose Subharmonicon_Firmware_v1_1_1.syx, not the ZIP, erase, or invalidate file.');
    }
    const text = document.querySelector('#firmware-data').textContent;
    if (text.length > 450000) throw new Error('The patch bundle is invalid. Please reload this page.');
    const bytes = await patchFirmware(new Uint8Array(await file.arrayBuffer()), JSON.parse(text));
    outputUrl = URL.createObjectURL(new Blob([bytes], {type: 'application/octet-stream'}));
    download.href = outputUrl;
    download.hidden = false;
    download.click();
    status.textContent = 'Your modified firmware is ready. Use Moog’s update instructions to install it.';
  } catch (error) {
    status.dataset.error = 'true';
    status.textContent = error instanceof Error ? error.message : 'Could not prepare the firmware. Please try again.';
  } finally {
    input.disabled = false;
    input.value = '';
  }
});
