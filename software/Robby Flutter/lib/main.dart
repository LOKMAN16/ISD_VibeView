import 'package:flutter/material.dart';
import 'package:camera/camera.dart';
import 'package:flutter/services.dart';
import 'package:permission_handler/permission_handler.dart';
//import 'package:provider/provider.dart';
import 'package:tflite/tflite.dart';
import 'package:flutter_tts/flutter_tts.dart';
//import 'package:just_audio/just_audio.dart';
//import 'package:speech_to_text/speech_to_text.dart';
import 'dart:async';
import 'dart:io';
//import 'package:typed_data/typed_data.dart';
import 'package:get/get.dart';
import 'package:image/image.dart' as img;
import 'package:google_speech/speech_client_authenticator.dart';
//import 'package:path_provider/path_provider.dart';
import 'package:sound_stream/sound_stream.dart';
import 'package:google_speech/google_speech.dart';
import 'package:usb_serial/transaction.dart';
import 'package:usb_serial/usb_serial.dart';
import 'package:rxdart/rxdart.dart';
import 'package:assets_audio_player/assets_audio_player.dart';



///For gyroscope
import 'package:vector_math/vector_math_64.dart' hide Colors;
//import 'package:motion_sensors/motion_sensors.dart';

void main() async {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  // This widget is the root of your application.
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Flutter Demo',
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      home: const MyHomePage(title: 'Flutter Demo Home Page'),
    );
  }
}

class MyHomePage extends StatefulWidget {
  const MyHomePage({super.key, required this.title});
  final String title;

  @override
  State<MyHomePage> createState() => _MyHomePageState();
}

/*class SpeechApi {
  static final _speech = SpeechToText();

  static Future<bool> toggleRecording({
    required Function(String text) onResult,
    required ValueChanged<bool> onListening,
  }) async {
    if (_speech.isListening) {
      _speech.stop();
      return true;
    }

    final isAvailable = await _speech.initialize(
      onStatus: (status) => onListening(_speech.isListening),
      onError: (e) => print('Error: $e'),
    );

    if (isAvailable) {
      _speech.listen(onResult: (value) => onResult(value.recognizedWords));
    }

    return isAvailable;
  }
}

 */
class _MyHomePageState extends State<MyHomePage> {
  ///Data to send to Arduino
  List<int> dataToArduino = List<int>.filled(6, 0);

  ///Gyroscope values
  final Vector3 _absoluteOrientation = Vector3.zero();
  String moveLR = "", moveUD = "";

  ///Serial settings
  UsbPort? _port;
  List<Widget> _ports = [];
  List<Widget> _serialData = [];

  StreamSubscription<String>? _subscription;
  Transaction<String>? _transaction;
  UsbDevice? _device;

  Future<bool> _connectTo(UsbDevice? device) async {
    _serialData.clear();

    if (_subscription != null) {
      _subscription!.cancel();
      _subscription = null;
    }

    if (_transaction != null) {
      _transaction!.dispose();
      _transaction = null;
    }

    if (_port != null) {
      _port!.close();
      _port = null;
    }

    if (device == null) {
      _device = null;
      setState(() {});
      return true;
    }

    _port = await device.create();
    if (await (_port!.open()) != true) {
      setState(() {});
      return false;
    }
    _device = device;

    await _port!.setDTR(true);
    await _port!.setRTS(true);
    await _port!.setPortParameters(
        115200, UsbPort.DATABITS_8, UsbPort.STOPBITS_1, UsbPort.PARITY_NONE);

    _transaction = Transaction.stringTerminated(
        _port!.inputStream as Stream<Uint8List>, Uint8List.fromList([13, 10]));

    _subscription = _transaction!.stream.listen((String line) {
      setState(() {
        _serialData.add(Text(line));
        if (_serialData.length > 20) {
          _serialData.removeAt(0);
        }
      });
    });

    setState(() {});
    return true;
  }

  void _getPorts() async {
    _ports = [];
    List<UsbDevice> devices = await UsbSerial.listDevices();
    if (!devices.contains(_device)) {
      _connectTo(null);
    }
    print("Devices123: $devices");

    for (var device in devices) {
      _ports.add(ElevatedButton(
        child: Text(_device == device ? "Disconnect" : "Connect"),
        onPressed: () {
          print("Hello123");
          print("Ports: $_ports");
          print("Devices: $devices");
          _connectTo(_device == device ? null : device).then((res) {
            _getPorts();
          });
        },
      ));
    }

    setState(() {
      print("Ports: $_ports");
    });
  }

  void connectToUSB() {
    print("Ports: $_ports");
    UsbSerial.usbEventStream!.listen((UsbEvent event) {
      _getPorts();
    });

    _getPorts();
  }

  ///End of serial settings

  final RecorderStream _recorder = RecorderStream();
  bool recognizing = false;
  bool recognizeFinished = false;

  void streamingRecognize() async {
    _audioStream = BehaviorSubject<List<int>>();
    _audioStreamSubscription = _recorder.audioStream.listen((event) {
      _audioStream!.add(event);
    });

    await _recorder.start();

    setState(() {
      recognizing = true;
    });
    final serviceAccount = ServiceAccount.fromString((await rootBundle
        .loadString('assets/singular-range-384407-6d248a6cf3cc.json')));
    final speechToText = SpeechToText.viaServiceAccount(serviceAccount);
    final config = _getConfig();

    final responseStream = speechToText.streamingRecognize(
        StreamingRecognitionConfig(config: config, interimResults: true),
        _audioStream!);

    var responseText = '';

    responseStream.listen((data) {
      final currentText =
          data.results.map((e) => e.alternatives.first.transcript).join('\n');

      if (data.results.first.isFinal) {
        responseText += '\n' + currentText;
        setState(() {
          text = responseText;
          recognizeFinished = true;
        });
      } else {
        setState(() {
          text = responseText + '\n' + currentText;
          recognizeFinished = true;
        });
      }
    }, onDone: () {
      setState(() {
        recognizing = false;
      });
    });
  }

  void stopRecording() async {
    await _recorder.stop();
    await _audioStreamSubscription?.cancel();
    await _audioStream?.close();
    setState(() {
      recognizing = false;
    });
  }

  RecognitionConfig _getConfig() => RecognitionConfig(
      encoding: AudioEncoding.LINEAR16,
      model: RecognitionModel.command_and_search,
      enableAutomaticPunctuation: false,
      sampleRateHertz: 16000,
      speechContexts: [sc],
      languageCode: 'en-US');

  int _counter = 0;
  int gesturecount = -1;
  late Future<void> _initializeControllerFuture;

  bool _isCameraPermissionGranted = false;
  String text = 'laptop';
  StreamSubscription<List<int>>? _audioStreamSubscription;
  BehaviorSubject<List<int>>? _audioStream;
  bool isListening = false;

  //late File _imageFile;
  late bool _audioReady = false;
  late FlutterTts flutterTts;
  List<String> words = [
    "bottle",
    "mouse",
    "cell phone",
    "laptop",
    "keys",
    "cup",
    "wallet",
    "headphones",
    "glasses",
    "remote",
    "orange",
    "spoon",
    "fork"
  ];
  SpeechContext sc = new SpeechContext([
    "bottle",
    "mouse",
    "cell phone",
    "laptop",
    "keys",
    "cup",
    "wallet",
    "headphones",
    "glasses",
    "remote",
    "orange",
    "spoon",
    "fork"
  ]);
  late File _imageFile;
  double confidenceThreshold = 0.1;
  final assetsAudioPlayer = AssetsAudioPlayer();
  late List<CameraDescription> cameras = [];
  late CameraController controller;
  CameraImage? _cameraImage;
  final RxBool _isCameraInitialized = RxBool(false);
  final RxList<Uint8List> _imageList = RxList([]);
  double angle = 0.0;
  double size = 0.0;
  double height = 0.0;
  int count = 0;
  int count2 = 0;
  FlutterTts fluttertts = FlutterTts();
  bool isCameraInitialized = false;

  CameraController get cameraController => controller;
  bool get isInitialized => _isCameraInitialized.value;
  List<Uint8List> get imageList => _imageList;

  Future getCameraPermission() async {
    print('getCameraPermission()');
    var status = await Permission.camera.request();
    print("request status: $status");
    if (status.isGranted || status.isLimited) {
      print('Camera Permission: GRANTED');
      setState(() {
        _isCameraPermissionGranted = true;
      });

      //onNewCameraSelected(cameras[0]);
    } else if (status.isPermanentlyDenied) {
      print('Camera Permission: PERMANENTLY DENIED');
      openAppSettings();
    } else {
      print('Camera Permission: DENIED');
    }
  }

  void onNewCameraSelected(CameraDescription cameraDescription) async {
    print('onNewCameraSelected');
    final previousCameraController = controller;

    final CameraController cameraController = CameraController(
      cameraDescription,
      ResolutionPreset.veryHigh,
      imageFormatGroup: ImageFormatGroup.jpeg,
      enableAudio: false,
    );
    await previousCameraController?.dispose();

    if (mounted) {
      setState(() {
        controller = cameraController;
      });
    }

    // Update UI if controller updated
    cameraController.addListener(() {
      if (mounted) setState(() {});
    });

    try {
      await cameraController
          .initialize()
          .then((_) => cameraController.setFlashMode(FlashMode.off));
    } on CameraException catch (e) {
      print('Error initializing camera: $e');
    }
  }

  Future<void> initCamera() async {
    cameras = await availableCameras();
    controller = CameraController(cameras[0], ResolutionPreset.high,
        imageFormatGroup: ImageFormatGroup.bgra8888);
    controller.initialize().then((value) {
      _isCameraInitialized.value = true;

      controller.startImageStream((image) {
        ;
        _counter++;
        if (_counter % 10 == 0) {
          _counter = 0;
          runModelOnFrame(image);
        }
      });
      if (mounted) {
        setState(() {
          isCameraInitialized = controller!.value.isInitialized;
        });
      }

      // });
    });
  }

  void Capture() {
    if (_cameraImage != null) {
      img.Image image = img.Image.fromBytes(_cameraImage!.width,
          _cameraImage!.height, _cameraImage!.planes[0].bytes,
          format: img.Format.bgra);
      Uint8List list = Uint8List.fromList(img.encodeJpg(image));
      _imageList.add(list);
      _imageList.refresh();
    }
  }

  /*Future toggleRecording() => SpeechApi.toggleRecording(
      onResult: (text) => setState(() => this.text = text),
      onListening: (isListening) {
        setState(() => this.isListening = isListening);

        if (!isListening) {
          Future.delayed(Duration(seconds: 1), () {
            print(text);
          });
        }
      });

   */
  Future loadModel() async {
    Tflite.close();
    try {
      print('TRYING TO LOAD TFLITE MODEL... ');
      String? res = await Tflite.loadModel(
        model: "assets/ssd_mobilenet.tflite",
        labels: "assets/ssd_mobilenet.txt",
      );
      print('LOADING MODEL RESULT: $res');
    } on PlatformException {
      print('Failed to load model.');
    }
  }

  Future<List<String>> detectObjects(File image) async {
    List<String> objects = [];
    debugPrint('FINDING OBJECTS IN IMAGE...');

    var recognitions = await Tflite.detectObjectOnImage(
        path: image.path,
        // required
        model: "SSDMobileNet",
        imageMean: 127.5,
        imageStd: 127.5,
        threshold: 0.4,
        // defaults to 0.1
        numResultsPerClass: 10,
        // defaults to 5
        asynch: true // defaults to true
        );
    FileImage(image).resolve(ImageConfiguration());
    debugPrint('THESE ARE THE OBJECTS... $recognitions');

    recognitions!.forEach((item) {
      var confidence = item[
          'confidenceInClass']; // item['confidence']; // item['confidenceInClass'];
      if (confidence > confidenceThreshold) {
        String label = item['detectedClass']
            .toUpperCase(); //item['detectedClass'].toUpperCase();
        objects.add(label);
      }
    });

    print(recognitions);
    return objects;
  }

  Future<List<dynamic>?> runModelOnFrame(CameraImage image) async {
    var recognitions = await Tflite.detectObjectOnFrame(
        bytesList: image.planes.map((plane) {
          return plane.bytes;
        }).toList(),
        // required
        model: "SSDMobileNet",
        imageHeight: image.height,
        imageWidth: image.width,
        imageMean: 127.5,
        // defaults to 127.5
        imageStd: 127.5,
        // defaults to 127.5
        rotation: 90,
        // defaults to 90, Android only
        // defaults to 5
        threshold: 0.1,
        // defaults to 0.1
        asynch: true // defaults to true
        );
    print(recognitions);

    if (text != '') {
      print(text.trim());
      for (var i = 0; i < recognitions!.length; i++) {
        if (recognitions[i]['detectedClass'] == text.trim().toLowerCase()) {
          setState(() => angle = (recognitions[i]['rect']['x'] +
                  recognitions[i]['rect']['w'] / 2) *
              94);
          setState(() => size =
              (recognitions[i]['rect']['w'] * recognitions[i]['rect']['h']) *
                  100);

          setState(() => height = (recognitions[i]['rect']['y'] +
                  recognitions[i]['rect']['h'] / 2) *
              100);

          count = 1;
          count2 = 0;

          ///Update dataToArduino according to object's position (left-right)
          if (angle < 7) {
            setState(() {
              moveLR = "(not visible)";
              dataToArduino[0] = 0;
              dataToArduino[1] = 0;
              dataToArduino[5] = 3;
            });
          } else if (angle >= 7 && angle < 14) {
            setState(() {
              moveLR = "left";
              dataToArduino[0] = 4;
              dataToArduino[1] = 0;
              dataToArduino[5] = 0;
            });
          } else if (angle >= 14 && angle < 21) {
            setState(() {
              moveLR = "left";
              dataToArduino[0] = 3;
              dataToArduino[1] = 0;
              dataToArduino[5] = 0;
            });
          } else if (angle >= 21 && angle < 28) {
            setState(() {
              moveLR = "left";
              dataToArduino[0] = 2;
              dataToArduino[1] = 0;
              dataToArduino[5] = 0;
            });
          } else if (angle >= 28 && angle < 35) {
            setState(() {
              moveLR = "left";
              dataToArduino[0] = 1;
              dataToArduino[1] = 0;
              dataToArduino[5] = 0;
            });
          } else if (angle >= 35 && angle < 42) {
            setState(() {
              moveLR = "left";
              dataToArduino[0] = 0;
              dataToArduino[1] = 0;
              dataToArduino[5] = 0;
            });
          } else if (angle >= 42 && angle < 50) {
            setState(() {
              moveLR = "";
              dataToArduino[0] = 0;
              dataToArduino[1] = 0;
              dataToArduino[5] = 5;
            });
          } else if (angle >= 50 && angle < 57) {
            setState(() {
              moveLR = "right";
              dataToArduino[0] = 0;
              dataToArduino[1] = 0;
              dataToArduino[5] = 0;
            });
          } else if (angle >= 57 && angle < 64) {
            setState(() {
              moveLR = "right";
              dataToArduino[0] = 0;
              dataToArduino[1] = 1;
              dataToArduino[5] = 0;
            });
          } else if (angle >= 64 && angle < 71) {
            setState(() {
              moveLR = "right";
              dataToArduino[0] = 0;
              dataToArduino[1] = 2;
              dataToArduino[5] = 0;
            });
          } else if (angle >= 71 && angle < 78) {
            setState(() {
              moveLR = "right";
              dataToArduino[0] = 0;
              dataToArduino[1] = 3;
              dataToArduino[5] = 0;
            });
          } else if (angle >= 78 && angle < 85) {
            setState(() {
              moveLR = "right";
              dataToArduino[0] = 0;
              dataToArduino[1] = 4;
              dataToArduino[5] = 0;
            });
          } else if (angle >= 85) {
            setState(() {
              moveLR = "(not visible)";
              dataToArduino[0] = 0;
              dataToArduino[1] = 0;
              dataToArduino[5] = 3;
            });
          }

          ///Update dataToArduino according to object's position (up-down)
          if (height < 9) {
            setState(() {
              moveUD = "up";
              dataToArduino[2] = 5;
            });
          } else if (height >= 9 && height < 18) {
            setState(() {
              moveUD = "up";
              dataToArduino[2] = 4;
            });
          } else if (height >= 18 && height < 27) {
            setState(() {
              moveUD = "up";
              dataToArduino[2] = 3;
            });
          } else if (height >= 27 && height < 36) {
            setState(() {
              moveUD = "up";
              dataToArduino[2] = 2;
            });
          } else if (height >= 36 && height < 45) {
            setState(() {
              moveUD = "up";
              dataToArduino[2] = 1;
            });
          } else if (height >= 45 && height < 55) {
            setState(() {
              moveUD = "forward";
              dataToArduino[2] = 0;
            });
          } else if (height >= 55 && height < 64) {
            setState(() {
              moveUD = "down";
              dataToArduino[2] = 1;
            });
          } else if (height >= 64 && height < 73) {
            setState(() {
              moveUD = "down";
              dataToArduino[2] = 2;
            });
          } else if (height >= 73 && height < 82) {
            setState(() {
              moveUD = "down";
              dataToArduino[2] = 3;
            });
          } else if (height >= 82 && height < 91) {
            setState(() {
              moveUD = "down";
              dataToArduino[2] = 4;
            });
          } else if (height >= 91) {
            setState(() {
              moveUD = "down";
              dataToArduino[2] = 5;
            });
          }

          ///Update dataToArduino according to object's distance
/*
          if (size >= 30) {
            setState(() {
              dataToArduino[2] = 2;
            });
          }
 */

          ///Update DataToArduino[5] for different vibration patterns
          if (count == 0 && count2 == 0) {
            setState(() => dataToArduino[5] = 3);
          } else if (angle >= 42 && angle < 50 && height >= 45 && height < 55) {
            setState(() {
              dataToArduino[2] = 5;
            });
          } else {
            setState(() {
              dataToArduino[2] = 0;
            });
          }

          break;
        } else {
          continue;
        }
      }

      if (count == 0 && count2 == 0) {
        //fluttertts.speak("object moved out of frame");
        count2 = 1;
        setState(() => angle = 0);
        setState(() => size = 0);
        setState(() => height = 0);
        moveLR = "(not visible)";
        moveUD = "";
      }
      Future.delayed(Duration(milliseconds: 1000), () {
        // Do something
      });
    }
    count = 0;
  }

  void _incrementCounter() {
    setState(() {
      _counter++;
    });
  }

  ///Send data to Arduino
  void sendDataToArduino() {

    Timer.periodic(const Duration(milliseconds: 250), (timer) {
      setState(() {
        _port!.write(Uint8List.fromList(dataToArduino));
        print(dataToArduino);
      });
    });
  }

  ///Gyroscope variables
  final gyro = List<double>.filled(3, 0.0);

  @override
  void initState() {
    super.initState();
    initCamera();
    loadModel();
    _recorder.initialize();
    FlutterTts fluttertts = new FlutterTts();

    // detectObject(_imageFile);
    flutterTts = FlutterTts();
    SystemChrome.setSystemUIOverlayStyle(
        SystemUiOverlayStyle(statusBarColor: Colors.transparent));
    getCameraPermission();

    ///Load USB ports
    UsbSerial.usbEventStream!.listen((UsbEvent event) {
      _getPorts();
    });
    _getPorts();

    /*
    ///Listen to gyroscope data
    motionSensors.absoluteOrientationUpdateInterval = 500000;
    motionSensors.absoluteOrientation.listen((AbsoluteOrientationEvent event) {
      setState(() {
        _absoluteOrientation.setValues(event.yaw, event.pitch, event.roll);
        gyro[0] = degrees(_absoluteOrientation.x);
        gyro[1] = degrees(_absoluteOrientation.y);
        gyro[2] = degrees(_absoluteOrientation.z);
        gyro[0] = gyro[0] < 0 ? 360 + gyro[0] : gyro[0];
        gyro[1] = gyro[1] < 0 ? 360 + gyro[1] : gyro[1];
        }
      });
    });

     */
  }

  // check permissions when app is resumed
  // this is when permissions are changed in app settings outside of app
  @override
  void didChangeAppLifecycleState(AppLifecycleState state) async {
    print('didChangeAppLifecycleState $state');
    if (controller == null || !controller!.value.isInitialized) return;
    if (state == AppLifecycleState.inactive) {
      print('app ianctive, disposing camera controller');
      controller?.dispose();
    }
    initCamera();
    if (state == AppLifecycleState.resumed) {
      setState(() {});
      print('app resumed');
      onNewCameraSelected(controller!.description);
    }
  }

  @override
  Widget build(BuildContext context) {
    WidgetsBinding.instance.addPostFrameCallback((_) {
      //setState(() {});
    });
    return SafeArea(
      top: false,
      child: Scaffold(
        backgroundColor: Colors.black54,
        extendBody: true,
        body: _isCameraPermissionGranted
            ? isCameraInitialized
                ? Stack(
                    children: <Widget>[
                      cameraViewFinder(context),
                      gesture(context),
                      Column(
                        mainAxisAlignment: MainAxisAlignment.start,
                        children: <Widget>[
                          objectName(context),
                          moveIndicator(context),
                        ],
                      ),
                      Column(
                        mainAxisAlignment: MainAxisAlignment.center,
                        children: <Widget>[
                          Row(
                            mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                            children: <Widget>[
                              sendDataToArduinoBtn(context),
                              connectUsbBtn(context),
                            ],
                          ),
                        ],
                      ),
                      Column(
                          mainAxisAlignment: MainAxisAlignment.center,
                          children: [/*voiceButton(context)*/]),
                    ],
                  )
                : Center(child: CircularProgressIndicator())
            : cameraPermissionAlert(context),
      ),
    );
  }

  Widget cameraViewFinder(BuildContext context) {
    print('camera view finder');

    double width = MediaQuery.of(context).size.width;
    double height = MediaQuery.of(context).size.height;

    return Container(
      child: CameraPreview(cameraController),
      width: width,
      height: height,
    );
  }

  Widget gesture(BuildContext context) {
    return GestureDetector(
        //onTap: recognizing ? stopRecording  : streamingRecognize);
    onTap:() {
      if (recognizing){
        stopRecording();
        assetsAudioPlayer.open(
          Audio("assets/start.mp3"),
        );
    }
      else {
        streamingRecognize();
        assetsAudioPlayer.open(
          Audio("assets/start.mp3"),
        );
      }
    },
    );
    /* onTap:() {
        gesturecount++;
        flutterTts.speak(words[gesturecount]);
        if (gesturecount>=8){
          gesturecount =-1;
        }
      },
      onDoubleTap:() {
        gesturecount--;
        flutterTts.speak(words[gesturecount]);
      },
      onLongPress: (){
        setState(() => text = words[gesturecount]);
      },

    );


       */
  }

  Widget cameraPermissionAlert(BuildContext context) {
    return Align(
      alignment: Alignment.center,
      child: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          Text('Camera Permission Denied!',
              style: TextStyle(fontSize: 20, color: Colors.white)),
          SizedBox(height: 20),
          ElevatedButton(
            style: TextButton.styleFrom(
              padding: const EdgeInsets.all(16.0),
              primary: Colors.white,
              textStyle: const TextStyle(fontSize: 20),
              alignment: Alignment.center,
            ),
            onPressed: getCameraPermission,
            child: const Text('Grant Camera Permission'),
          ),
        ],
      ),
    );
  }

  Widget objectName(BuildContext context) {
    return Row(
      mainAxisAlignment: MainAxisAlignment.center,
      children: [
        textDisplay(context),
      ],
    );
  }

  final double ACTION_BUTTON_WIDTH = 100;
  /*  Widget objectDetectionButton(BuildContext context) {

      return Container(
        height: 100,
        width: ACTION_BUTTON_WIDTH,
        padding: EdgeInsets.only(right: 0, bottom: 30),
        child: ElevatedButton(
          onPressed:  () async {

            HapticFeedback.heavyImpact();
            //
            // await _controller.initialize();

            //final image1 = await controller!.takePicture();
            //_imageFile = File(image1.path);
            //detectObjects(_imageFile);
            //runModelOnFrame(_cameraImage);
            //debugPrint('taking picture: ${image1.path}');
            //File img = File(image.path);
            //var decodedImage = await decodeImageFromList(img.readAsBytesSync());
            //print(decodedImage.width);
            //print(decodedImage.height);

          } ,





          //icon: Icon(Icons.follow_the_signs, color: Colors.black),
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Text(
                'Detect',
                style: TextStyle(fontSize: 19, color: Colors.black),
              )
            ],
          ),

          style: ElevatedButton.styleFrom(
            //primary: provider.tries! > 0 ? kPrimaryColor : kDisabledButtonColor,
            shape:
            RoundedRectangleBorder(borderRadius: BorderRadius.circular(18)),
          ),
        ),
      );
    }

   */

  Widget textDisplay(BuildContext context) {
    return Container(
      height: 100,
      width: 400,
      padding: const EdgeInsets.only(right: 0, top: 50),
      child: ElevatedButton(
        onPressed: () async {
          if (angle != 0) {
            fluttertts.speak("object found");
          }
        },

        //icon: Icon(Icons.follow_the_signs, color: Colors.black),
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            Text(
              "$text ${angle.round()} ${height.round()} ${size.round()}",
              style: const TextStyle(fontSize: 19, color: Colors.black),
            ),
          ],
        ),
      ),
    );
  }

  Widget moveIndicator(BuildContext context) {
    return Row(mainAxisAlignment: MainAxisAlignment.center, children: [
      Container(
          height: 60,
          width: 400,
          padding: const EdgeInsets.only(right: 0, top: 10),
          child: ElevatedButton(
            onPressed: () {},
            style: ElevatedButton.styleFrom(
                backgroundColor: moveLR == "(not visible)"
                    ? Colors.pink[200]
                    : Colors.green),
            child:
                Column(mainAxisAlignment: MainAxisAlignment.center, children: [
              Text(
                "Move $moveLR $moveUD",
                style: const TextStyle(fontSize: 19, color: Colors.black),
              )
            ]),
          )),
    ]);
  }

  Widget voiceButton(BuildContext context) {
    return Positioned(
      bottom: 100,
      child: Container(
        height: 100,
        width: ACTION_BUTTON_WIDTH,
        padding: const EdgeInsets.only(left: 0, bottom: 0),
        child: ElevatedButton(
          onPressed: recognizing ? stopRecording : streamingRecognize,
          style: ElevatedButton.styleFrom(
            shape:
                RoundedRectangleBorder(borderRadius: BorderRadius.circular(18)),
          ),

          // toggleRecording();

          //icon: Icon(Icons.text_snippet, color: Colors.black),
          child: const Icon(
            Icons.mic,
            color: Colors.white,
            size: 50.0,
          ),
        ),
      ),
    );
  }

  /*Widget button1(BuildContext context) {
    return Positioned( //<-- SEE HERE
        right: 20,
        top: 300,
        child: Container(
          width: 400,
          height: 250,
          color: Colors.deepPurpleAccent,
          child: ElevatedButton(
            onPressed: () {
              flutterTts.speak("mouse");

            },
            onLongPress: () {setState(() => text = "mouse"); },
            //icon: Icon(Icons.text_snippet, color: Colors.black),
            child:
            Text(

                "mouse"
            ),
            style: ElevatedButton.styleFrom(

              shape:
              RoundedRectangleBorder(borderRadius: BorderRadius.circular(18)),
            ),
          ),));
  }
  Widget button2(BuildContext context) {
     return Positioned( //<-- SEE HERE
    right: 20,
    top: 15,
    child: Container(
    width: 400,
    height: 500,
    color: Colors.deepPurpleAccent,
    child: ElevatedButton(
    onPressed: () {
    flutterTts.speak("laptop");

    },
      onLongPress: () {setState(() => text = "laptop"); },
    //icon: Icon(Icons.text_snippet, color: Colors.black),
    child:
    Text(

        "laptop"
    ),
    style: ElevatedButton.styleFrom(

    shape:
    RoundedRectangleBorder(borderRadius: BorderRadius.circular(18)),
    ),
    ),));

  }


*/

  Widget sendDataToArduinoBtn(BuildContext context) {
    return Container(
      width: 180,
      height: 50,
      color: Colors.amber,
      child: ElevatedButton(
        onPressed: _port == null
            ? null
            : () async {
                if (_port == null) {
                  return;
                }
                sendDataToArduino();
              },
        style: ElevatedButton.styleFrom(
          shape:
              RoundedRectangleBorder(borderRadius: BorderRadius.circular(18)),
          backgroundColor: Colors.amber,
        ),
        //icon: Icon(Icons.text_snippet, color: Colors.black),
        child: const Text(
          "Send",
          style: TextStyle(fontSize: 20, fontWeight: FontWeight.bold),
        ),
      ),
    );
  }

  Widget connectUsbBtn(BuildContext context) {
    return Container(
      width: 180,
      height: 50,
      color: _ports.isNotEmpty ? Colors.green : Colors.pink[200],
      child: Center(
          child: _ports.isNotEmpty
              ? _ports[0]
              : const Text(
                  "No USB",
                  style: TextStyle(fontSize: 20, fontWeight: FontWeight.bold),
                )),
    );
  }

/*
  Widget loadUsbBtn(BuildContext context) {
    return Positioned(
        //<-- SEE HERE
        right: 20,
        top: 680,
        child: Container(
          width: 150,
          height: 100,
          color: Colors.deepPurpleAccent,
          child: ElevatedButton(
            onPressed: () {
              connectToUSB();
            },
            style: ElevatedButton.styleFrom(
              shape: RoundedRectangleBorder(
                  borderRadius: BorderRadius.circular(18)),
            ),
            child: const Text("Connect"),
          ),
        ));
  }

   */
}
