plugins {
    id("com.android.library")
}

private val versionMajor = 4
private val versionMinor = 2
private val versionPatch = 1

var libraryVersionName by extra("${versionMajor}.${versionMinor}.${versionPatch}")
var libraryVersionCode by extra((versionMajor * 10000) + (versionMinor * 100) + versionPatch)
var libraryArtifactId by extra("applovin-max-unity-plugin")
var libraryArtifactName by extra("${libraryArtifactId}-${libraryVersionName}.aar")

var libraryVersions = rootProject.extra["versions"] as Map<*, *>

android {
    compileSdkVersion(libraryVersions["compileSdk"] as Int)

    defaultConfig {
        minSdkVersion(libraryVersions["minSdk"] as Int)
        targetSdkVersion(libraryVersions["targetSdk"] as Int)
        versionCode = libraryVersionCode
        versionName = libraryVersionName

        consumerProguardFiles("proguard-rules.pro")
    }

    flavorDimensions("default")
    productFlavors {
        // Flavor when building Unity Plugin as a standalone product
        create("standalone") {
            buildConfigField("boolean", "IS_TEST_APP", "false")
        }
        // Flavor from the test app
        create("app") {
            buildConfigField("boolean", "IS_TEST_APP", "true")
        }
    }

    buildTypes {
        getByName("debug") {
            isMinifyEnabled = false
        }
        getByName("release") {
            isMinifyEnabled = false
            proguardFiles(getDefaultProguardFile("proguard-android.txt"), "proguard-rules.pro")
        }
    }
}

dependencies {

    // AppLovin Workspace SDK
    if (file("../../../SDK-Android/Android-SDK/build.gradle.kts").exists()){
        compileOnly(project(":Android-SDK"))
    }
    else {
        compileOnly ("com.applovin:applovin-sdk:+@aar")
    }

    // Unity
    compileOnly("com.unity.player:android-sdk:1.0@jar")
}

repositories {
    google()
    mavenCentral()
    jcenter()

    flatDir {
        dirs("libs")
    }
}
