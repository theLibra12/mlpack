#ifndef LOSSFUNCTIONS_H_
#define LOSSFUNCTIONS_H_

class LossFunctions {

public:
  // Returns the loss value
  virtual double getLoss(double prediction, double label) = 0;
  
  // Returns the update scalar
  virtual double getUpdate(double prediction, double label) = 0;

  // Returns the name of loss function
  virtual string getName() const {
    return "";
  }
  
  virtual ~LossFunctions() {};
};


class squaredloss : public LossFunctions {
 public:
  squaredloss() {  
    // construction
  }
  
  double getLoss(double prediction, double label) {
    double example_loss = (prediction - label) * (prediction - label);
    return example_loss;
  }
  
  double getUpdate(double prediction, double label) {
    return (label - prediction);
  }

  string getName() const {
    return "squared";
  }
};

class hingeloss : public LossFunctions {
 public:
  hingeloss() {
    // construction
  }
  
  double getLoss(double prediction, double label) {
    double e = 1 - label*prediction;
    return (e > 0) ? e : 0;
  }
  
  double getUpdate(double prediction, double label) {
    if ( (prediction * label) < 1.0)
      return label;  
    else
      return 0.0;
  }

  string getName() const {
    return "hinge";
  }
};

class squaredhingeloss : public LossFunctions {
 public:
  squaredhingeloss() {
    // construction
  }
  
  double getLoss(double prediction, double label) {
    double e = 1 - label*prediction;
    return (e > 0) ? 0.5*e*e : 0;
  }
  
  double getUpdate(double prediction, double label) {
    double e = 1 - label*prediction;
    return (e > 0) ? e : 0;
  }

  string getName() const {
    return "squaredhinge";
  }
};

class logloss : public LossFunctions {
 public:
  logloss() {
    // construction
  }
  
  double getLoss(double prediction, double label) {
    return log(1 + exp(-label * prediction));
  }
  
  double getUpdate(double prediction, double label) {
    double d = exp(-label * prediction);
    return label * d / (1 + d);
  }

  string getName() const {
    return "logistic";
  }
};

class quantileloss : public LossFunctions {
 public:
  quantileloss(double &tau_) : tau(tau_) {
    // construction
  }
  
  double getLoss(double prediction, double label) {
    double e = label - prediction;
    if(e > 0) {
      return tau * e;
    } else {
      return -(1 - tau) * e;
    }
    
  }
  
  double getUpdate(double prediction, double label) {
    double e = label - prediction;
    if(e == 0) return 0;
    if(e > 0) {
      return tau;
    } else {
      return -(1 - tau);
    }
  }
  
  double tau;

  string getName() const {
    return "quantile";
  }
};


LossFunctions* getLossFunction(string funcName, double funcPara) {
  if(funcName.compare("squared") == 0) {
    return new squaredloss();
  }
  else if(funcName.compare("hinge") == 0) {
    return new hingeloss();
  }
  else if(funcName.compare("squaredhinge") == 0) {
    return new squaredhingeloss();
  }
  else if(funcName.compare("logistic") == 0) {
    return new logloss();
  }
  else if(funcName.compare("quantile") == 0 || funcName.compare("pinball") == 0 || funcName.compare("absolute") == 0) {
    return new quantileloss(funcPara);
  }
  else {
    cout << "Invalid loss function name: " << funcName << ". Bailing!" << endl;
    exit(1);
  }
}

#endif

