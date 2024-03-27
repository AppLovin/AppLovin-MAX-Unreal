plugins {
    id("com.android.library")
}

var libraryVersions = rootProject.extra["versions"] as Map<*, *>

android {
    namespace = "com.applovin.unreal"
    compileSdkVersion(libraryVersions["compileSdk"] as Int)

    defaultConfig {
        minSdkVersion(libraryVersions["minSdk"] as Int)

        consumerProguardFiles("proguard-rules.pro")
    }

    flavorDimensions("default")
    productFlavors {
        // Flavor when building Unreal Plugin as a standalone product
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
}

repositories {
    google()
    mavenCentral()

    flatDir {
        dirs("libs")
    }
}
