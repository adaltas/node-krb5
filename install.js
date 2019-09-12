const { execSync, spawn } = require('child_process');
const os = require('os');
const platform = os.platform();

function installer() {
    const installString = 'node-gyp rebuild';
    if (!process.env.MANUAL && platform == 'os390') {
        execSync(". ./install_krb5_zos.sh && node-gyp rebuild", { stdio: 'inherit' }); 
    } else {
        spawn("node-gyp", ["rebuild"], { stdio: 'inherit' });     
    }
}

installer();
