import UIKit

@UIApplicationMain
class AppDelegate: UIResponder, UIApplicationDelegate {

    var window: UIWindow?

    private func playSound()
    {
        let bank = CkoBank.newBank("hellocricket.ckb")
        let sound = CkoSound.newBankSound(bank, index: 0)!
        sound.play()

        while (sound.playing)
        {
            CkUpdate()
            usleep(30000)
        }
    }

    func application(_ : UIApplication, didFinishLaunchingWithOptions launchOptions: [UIApplicationLaunchOptionsKey : Any]?) -> Bool
    {
        var config = CkConfig()
        CkConfigInit(&config)
        CkInit(&config)

        playSound()

        CkShutdown()

        return true
    }


}

