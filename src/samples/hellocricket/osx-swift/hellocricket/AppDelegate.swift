import Cocoa

@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate {

    @IBOutlet weak var window: NSWindow!

    fileprivate func playSound()
    {
        let bank = CkoBank.newBank("hellocricket.ckb")
        let sound = CkoSound.newBankSound(bank, index: 0)
        sound?.play()

        while (sound?.playing)!
        {
            CkUpdate()
            usleep(30000)
        }
    }

    func applicationDidFinishLaunching(_ aNotification: Notification) {
        // Insert code here to initialize your application

        var config = CkConfig()
        CkConfigInit(&config)
        CkInit(&config)

        playSound()

        CkShutdown()
    }

    func applicationWillTerminate(_ aNotification: Notification) {
        // Insert code here to tear down your application
    }


}

