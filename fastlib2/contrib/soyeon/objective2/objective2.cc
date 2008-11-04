#include "objective.h"
#include <cmath>
    
Objective::Init(fx_module *module) {

}

Objective::ComputeObjective(Matrix &x, double *objective) {
  *objective = ComputeTerm1_() + ComputeTerm2_() + ComputeTerm3_();
}

double Objective::ComputeTerm1_(Vector &betas) {
  double term1=0;
  for(index_t n=0; n<first_stage_x_.size(); n++) {
    if (first_stage_y_[n]<0) { 
			//first_stage_y_[n]=-1 if all==zero, j_i is n chose j_i
      continue;
    } else {
      Vector temp;
      first_stage_x_[n].MakeColumnVector(first_stage_y_[n], &temp);
			term1+=la::Dot(betas, temp) - log(exp_betas_times_x1_[n]);
    }
  }
  return term1;
}

double Objective::ComputeTerm2_(Vector &betas, double p, double q) {
  double term2=0;
  for(index_t n=0; n<first_stage_x_.size(); n++) {
    if (first_stage_y_[n]<0) {
      continue;
    } else {
      DEBUG_ASSERT(1-postponed_probability_[n]);
      term2+=log(1-postponed_probability_[n]);
    }
  }
  return term2;
}

double Objecitve::ComputeTerm3_() {
  double term3=0;
  for(index_t n=0; n<first_stage_x_.size(); n++) {
    if (second_stage_y_[n]<0) {
      continue;
    } else {
      DEBUG_ASSERT(postponed_probability_[n]>0);
      term3+=log(postponed_probability_[n]);
    }
  }
  return term3;
}

//Compute x^2_{ni}(alpha), beta'x^2_{ni}(alpha), and postponedprob.
void Objective::ComputePostponedProbability_(Vector &betas, 
                                             double p, 
                                             double q) {
  postponed_probability_.SetZero(); 

	double alpha_temp=0;
	double beta_function_temp=0;
	double numerator=0;
	//need to specify
	num_of_alphas_=10;
	alpha_weight_=1/num_of_alphas;

	exp_betas_times_x2_.SetZero();
  
	for(index_t n=0; n<first_stage_x_.size; n++){
		for(index_t l=0; l<num_of_alphas-1; ;++){
			alpha_temp=(l+1)*(alpha_weight_);
		
			beta_function_temp=pow(alpha_temp, p-1)*pow((1-alpha_temp), q-1)/denumerator_beta_function_;

		
			//Calculate x^2_{ni}(alpha_l)
			for(index_t i=0; i<first_stage_x_[n].n_cols(); i++){
				int count=0;
				for(index_t j=ind_unk_x_[0]; j<ind_unk_x_[ind_unk_x_.size()]; j++){
					count+=1;
					exponential_temp=alpha_temp*first_stage_x_[n].get(i, j)
													+(alpha_temp)*(1-alpha_temp)*unk_x_past[i].get(count-1,1)
													+(alpha_temp)*pow((1-alpha_temp),2)*unk_x_past[i].get(count-1,2);
					second_stage_x_[n].set(j, i, exponential_temp);
				}	//j
			}	//i

			for(index_t i=0; i<second_stage_x_[n].n_cols(); i++) {
				exp_betas_times_x2_[n]+=exp(la::Dot(betas.size(), betas.ptr(),
											 second_stage_x_[n].GetColumnPtr(i) ));
			}
			//conditional_postponed_probability_[n]
			postponed_probability_[n]+=( (exp_betas_times_x2_[n]/(exp_betas_times_x1_[n]
																  + exp_betas_times_x2_[n]) )
																*beta_function_temp );			
		}	//alpha
		postponed_probability_[n]*=alpha_wieght_;	
	}	//n

}

void Objective::ComputeExpBetasTimesX1_(Vector &betas) {
  exp_betas_times_x1_.SetZero();
  //double sum=0;
	for(index_t n=0; n<first_stage_x_.size(); n++){
		for(index_t j=0; j<first_stage_x_[n].n_cols(); j++) {
			exp_betas_times_x1_[n]+=exp(la::Dot(betas.length(), 
															beta.ptr(), 
															first_stage_y_[n].GetColumnPtr(j)));
		}
  }
}


void Objective::ComputeDeumeratorBetaFunction_(double p, doulbe q) {
	denumerator_beta_function_=0;
	//Need to choose number of t points to approximate integral
	num_of_t_beta_fn_=10;
	double t_weight_=1/(num_of_t_beta_fn);
	double t_temp;
	for(index t tnum=0; tnum<num_of_t_beta_fn_-1; tnum++){
		t_temp=(tnum+1)*(t_weight_);

		//double pow( double base, double exp );
		denumerator_beta_function_+=pow(t_temp, p-1)*pow((1-t_temp), q-1);
	}
	denumerator*=(t_weight_);
}

//////////////////////////////////////////////////////////
//add new things from here for objective2 (Compute gradient) 
//Compute dot_logit
void Objective::ComputeDotLogit_(Vector &betas) {
	for(index_t n=0; n<first_stage_x_.size(); n++){
		for(index_t i=0; i<first_stage_x_.n_cols(); i++){
			first_stage_dot_logit_[n].set(i, 1, exp(la::Dot(betas.length(), beta.ptr(),
																				 (first_stage_x_[n].GetColumnPtr(i))/
																				 exp_betas_times_x1_[n]));
		}	//i
		/*for(index_t j=0; j<second_stage_x_.n_cols(); j++){
			second_stage_dot_logit_[n].set(j, 1, exp(la::Dot(betas.length(), beta.ptr(),
																				 (second_stage_x_[n].GetColumnPtr(i))/
																				 exp_betas_times_x2_[n]));
																				 
		}	//j
		*/
	}	//n
}


void Objective::ComputeDDotLogit_() {
	first_stage_ddot_logit_.SetZero();
	second_stage_ddot_logit_.SetZero();

	for(index_t n=0; n<first_stage_x_.size(); n++){
		for(index_t i=0; i<first_stage_x_.n_cols(); i++){
			first_stage_ddot_logit_[n].set(i, i, first_stage_dot_logit_[n].get(i,1));
		}	//i
		/*for(index_t j=0; i<second_stage_x_.n_cols(); j++){
			second_stage_ddot_logit_[n].set(j, j, second_stage_dot_logit_[n].get(j,1));
		}	//j*/

	}	//n

}


Vector Objective::ComputeDerivativeBetaTerm1_() {
	Vector derivative_beta_term1;
	derivative_beta_term1.Init(betas.length());
	derivative_beta_term1.SetZero();

  for(index_t n=0; n<first_stage_x_.size(); n++) {
    if (first_stage_y_[n]<0) { 
			//first_stage_y_[n]=-1 if all==zero, j_i is n chose j_i
      continue;
    } else {
      Vector temp;
			temp.Init(betas.length());
			la::MulOverwrite(first_stage_x_[n], first_stage_dot_logit[n]), &temp);
			la::SubOverwrite(first_stage_x_[n].GetColumnPtr(first_stage_y_[n]), &temp);
			//check
			la::Addto(derivative_beta_term1, &temp);
																							
		}
  }
  return derivative_beta_term1;
}


/*void Objective::ComputeDerivativeBetaConditionalPostponedProb_(Vector &betas){
	derivative_beta_conditional_postponed_prob_.SetZero();
	conditional_postponed_prob_.SetZero();

	//Vector temp;
	//temp.Init(betas.lentgh());
	//temp.SetZero();

	for(index_t n=0; n<first_stage_x_[n].size; n++){
		
		conditional_postponed_prob_[n]=exp_betas_times_x2_[n]/(exp_betas_times_x1_[n]+exp_betas_times_x2_[n]);
		DEBUG_ASSERT(conditional_postponed_prob_[n]>0);
		DEBUG_ASSERT(conditional_postponed_prob_[n]<1);
		la::MulOverwrite(first_stage_x_[n], first_stage_dot_logit[n]), &derivative_beta_conditional_postponed_prob_[n]);
		//check
		la::MulExpert(1, second_stage_x_[n], second_stage_dot_logit[n], -1, &derivative_beta_conditional_postponed_prob_[n]);
		la::scale( conditional_postponed_prob_*(1-conditional_postponed_prob_), &derivative_beta_conditional_postponed_prob_[n]);
		//derivative_beta_conditional_postponed_prob_[n]=&temp;
	}
}
*/

void Objective::ComputeSumDerivativeConditionalPostpondProb_(Vector &betas){
	
	Vector temp;
	temp.Init(betas.length());	//dotX1*dotLogit1
	//SumSecondDerivativeConditionalPostpondProb_.SetZero();

	double alpha_temp=0;
	double beta_function_temp=0;
	double numerator=0;
	//need to specify
	num_of_alphas_=10;
	alpha_weight_=1/num_of_alphas;

	exp_betas_times_x2_.SetZero();
	second_stage_dot_logit_.SetZero();
	double conditional_postponed_prob=0;
	Vector first_derivative_conditional_postpond_prob;
	first_derivative_conditional_postpond_prob.Init(betas.length());

	Matrix second_derivative_conditional_postpond_prob;
	second_derivative_conditional_postpond_prob.Init(betas.length(),betas.length() );


	Vector temp2;	//dotX2*dotLogit2
	temp2.Init(betas.length());

	Matrix first_term_temp;
	first_term_temp.Init(betas.length(), betas.length());

	Matrix temp3; //dotLogit2*dotLogit2'
	temp3.Init(second_stage_x_[n].n_cols(), second_stage_x_[n].n_cols());

	Matrix temp4; //ddotLogit2-dotLogit2*dotLogit2'
	temp4.Init(second_stage_x_[n].n_cols(), second_stage_x_[n].n_cols());

	Matrix temp5;	//dotX2(temp4)
	temp5.Init(betas.length(), second_stage_x_[n].n_cols());

	Matrix temp6; //temp5*dotX2'
	temp6.Init(betas.length(), betas.length());

	//terms for the first stage
	Matrix temp7; //dotLogit1*dotLogit1
	temp7.Init(first_stage_x_[n].n_cols(), first_stage_x_[n].n_cols());

	Matrix temp8; //ddotLogit1-dotLogit1*dotLogit1'
	temp8.Init(first_stage_x_[n].n_cols(), first_stage_x_[n].n_cols());

	Matrix temp9;	//dotX1(temp8)
	temp5.Init(betas.length(), second_stage_x_[n].n_cols());

	Matrix temp10; //temp9*dotX1'
	temp10.Init(betas.length(), betas.length());

	Matrix second_term_temp;
	second_term_temp.Init(betas.length(), betas.length());






	for(index_t n=0; n<first_stage_x_.size(); n++){
		la::MulOverwrite(first_stage_x_[n], first_stage_dot_logit_[n], &temp);
		sum_first_derivative_conditional_postpond_prob_[n].SetZero();

		la::MulTransBOverwrite(first_stage_dot_logit_[n], first_stage_dot_logit_[n], &temp7);

		la::SubOverwrite(temp7, first_stage_ddot_logit_[n], &temp8);

		la::MulOverwrite(first_stage_dot_logit_[n], temp8, &temp9);

		la::MulTransBOverwrite(temp9, first_stage_dot_logit_[n], &temp10);



		for(index_t l=0; l<num_of_alphas-1; l++){
			alpha_temp=(l+1)*(alpha_weight_);
		
			beta_function_temp=pow(alpha_temp, p-1)*pow((1-alpha_temp), q-1)/denumerator_beta_function_;

		
			//Calculate x^2_{ni}(alpha_l)
			for(index_t i=0; i<first_stage_x_[n].n_cols(); i++){
				int count=0;
				for(index_t j=ind_unk_x_[0]; j<ind_unk_x_[ind_unk_x_.size()]; j++){
					count+=1;
					exponential_temp=alpha_temp*first_stage_x_[n].get(i, j)
													+(alpha_temp)*(1-alpha_temp)*unk_x_past[i].get(count-1,1)
													+(alpha_temp)*pow((1-alpha_temp),2)*unk_x_past[i].get(count-1,2);
					second_stage_x_[n].set(j, i, exponential_temp);
				}	//j
			}	//i

			for(index_t i=0; i<second_stage_x_[n].n_cols(); i++) {
				exp_betas_times_x2_[n]+=exp(la::Dot(betas.size(), betas.ptr(),
											 second_stage_x_[n].GetColumnPtr(i) ));
				//Calculate second_stage_dot_logit_
				second_stage_dot_logit_[n].set( i, 1, exp(la::Dot(betas.length(), beta.ptr(),
																				 (second_stage_x_[n].GetColumnPtr(i))/
																				 exp_betas_times_x2_[n]) );
				second_stage_ddot_logit_[n].set(i, i, first_stage_dot_logit_[n].get(i,1));
			}	//i
			conditional_postponed_prob=exp_betas_times_x2_[n]/(exp_betas_times_x1_[n]+exp_betas_times_x2_[n]);
			la::MulOverwrite(second_stage_x_[n], second_stage_dot_logit_[n], &temp2);
			la::SubOverwrite(temp2, temp1, &first_derivative_conditional_postpond_prob);

			//Calculate SecondDerivativePostponedProb.
			//Matrix first_term_temp;
			//first_term_temp.Init(betas.length(), betas.length());
			la::MulTransBOverwrite(first_derivative_conditional_postpond_prob, first_derivative_conditional_postpond_prob, 
														 &first_term_temp);
			la::Scale( (1-2*conditional_postponed_prob)*(conditional_postponed_prob)*(1-conditional_postponed_prob),
								&first_term_temp);

			//check
			//Matrix temp3; //dotLogit*dotLogit'
			//temp3.Init(second_stage_x_[n].n_cols(), second_stage_x_[n].n_cols());
			la::MulTransBOverwrite(second_stage_dot_logit_[n], second_stage_dot_logit_[n], &temp3);
			
			//Matrix temp4; //ddotLogit-dotLogit*dotLogit'
			//temp4.Init(second_stage_x_[n].n_cols(), second_stage_x_[n].n_cols());
			la::SubOverwrite(temp3, second_stage_ddot_logit_[n], &temp4);

			//Matrix temp5;	//dotX2(temp4)
			//temp5.Init(betas.length(), second_stage_x_[n].n_cols());
			la::MulOverwrite(second_stage_dot_logit_[n], temp4, &temp5);

			//Matrix temp6; //temp5*dotX2'
			//temp6.Init(betas.length(), betas.length());
			la::MulTransBOverwrite(temp5, second_stage_dot_logit_[n], &temp6);

			la::SubOverwrite(temp10, temp6, &second_term_temp);
			la::Scale( (conditional_postponed_prob)*(1-conditional_postponed_prob), &second_term_temp);

			la::AddOverwrite(second_term_temp, first_term_temp, &second_derivative_conditional_postpond_prob);
			//end of calculation of second_derivative_conditional_postpond_prob
			
			//Scale with beta_function
			la::Scale( beta_function_temp, &second_derivative_conditional_postpond_prob );
			//check
			la::Addto(sum_second_derivative_conditional_postpond_prob_[n], &second_derivative_conditional_postpond_prob);


			la::Scale( (conditional_postponed_prob)*(1-conditional_postponed_prob), &first_derivative_conditional_postpond_prob);

			//Scale with beta_function
			la::Scale( beta_function_temp, &first_derivative_conditional_postpond_prob );

			//Check
			la::Addto(sum_first_derivative_conditional_postpond_prob_[n], &first_derivative_conditional_postpond_prob);
					
		}	//alpha
		la::Scale(alpha_wieght_, &sum_first_derivative_conditional_postpond_prob_[n]);
		la::Scale(alpha_wieght_, &sum_second_derivative_conditional_postpond_prob_[n]);
	}	//n

}



Vector Objective::ComputeDerivativeBetaTerm2_() {

	derivative_beta_term2.Init(betas.length());
	derivative_beta_term2.SetZero();
	Vector temp;
	temp.Init(betas.length());

	for(index_t n=0; n<first_stage_x_.size(); n++){
		if (first_stage_y_[n]<0) {
      continue;
    } else {
			//check
			temp=SumFirstDerivativeConditionalPostpondProb_[n]/(1-postponed_probability_[n]);
			//check
			la::Addto(derivative_beta_term2, &temp);

		}	//if-else

	}	//n
	return derivative_beta_term2;

}

Matrix Objective::ComputeSecondDerivativeBetaTerm1_() {
	//check
	Matrix second_derivative_beta_term1;
	second_derivative_beta_term1.Init(betas.length(), betas.length());
	second_derivative_beta_term1.SetAll(0.0);

	Vector temp1;
	temp1.Init(betas.length());

	Matrix temp2
	temp2.Init(betas.length(), betas.length());

	Matrix temp3;
	temp3.Init(betas.length(), first_stage_x_[n].n_cols());

	Matrix temp4;
	temp3.Init(betas.length(), betas.length());

  for(index_t n=0; n<first_stage_x_.size(); n++) {
    if (first_stage_y_[n]<0) { 
			//first_stage_y_[n]=-1 if all==zero, j_i is n chose j_i
      continue;
    } else {

			//check from here
      //Vector temp1;
			//temp1.Init(betas.length());
			la::MulOverwrite(first_stage_x_[n], first_stage_dot_logit[n]), &temp1);

			//Matrix temp2
			//temp2.Init(betas.length(), betas.length());
			la::MulTransBOverwrite(temp1, temp1, &temp2);

			//Matrix temp3;
			//temp3.Init(betas.length(), first_stage_x_[n].n_cols());
			la::MulOverwrite(first_stage_x_[n], first_stage_ddot_logit_[n], &temp3);

			//Matrix temp4;
			//temp3.Init(betas.length(), betas.length());
			la::MulTransBOverwrite(temp3, first_stage_x_[n], &temp4);
			//check
			la::SubFrom(temp4, &temp2);
			la::Addto(second_derivative_beta_term1, &temp2);
																							
		}
  }
  return second_derivative_beta_term1;

}


Matrix Objective::ComputeSecondDerivativeBetaTerm2_() { 

	Matrix second_derivative_beta_term2;
	second_derivative_beta_term2.Init(betas.length(), betas.length());
	second_derivative_beta_term2.SetAll(0.0);

	Matrix first_temp;
	first_temp.Init(betas.length(), betas.length());

	Matrix second_temp;
	second_temp.Init(betas.length(), betas.length());

	Matrix second_derivative_beta_temp;
	second_derivative_beta_temp.Init(betas.length(), betas.length());

	for(index_t n=0; n<first_stage_x_.size(); n++){
		  if (first_stage_y_[n]<0) { 
				continue;
			} else {
				la::Scale( (1/(1-postponed_probability_[n]), &first_temp);
				la::MulTransBOverwrite(sum_first_derivative_conditional_postpond_prob_[n], 
															 sum_first_derivative_conditional_postpond_prob_[n], &second_temp);
				la::Scale( 1/pow((1-postponed_probability_[n]), 2), &second_temp);

				la::AddOverwrite(second_temp, first_temp, &second_derivative_beta_temp);

				//check
				la::AddTo(second_derivative_beta_temp, &second_derivative_beta_term2);

							
			}	//else

	}	//n
	la::Scale(-1, &second_derivative_beta_term2);
	return second_derivative_beta_term2;
}



Vector Objective::ComputeDerivativeBetaTerm3_() {
	derivative_beta_term3.Init(betas.length());
	derivative_beta_term3.SetZero();
	Vector temp;
	temp.Init(betas.length());

	for(index_t n=0; n<first_stage_x_.size(); n++){
		if (second_stage_y_[n]<0) {
      continue;
    } else {
			//check
			temp=SumFirstDerivativeConditionalPostpondProb_[n]/(postponed_probability_[n]);
			//check
			la::Addto(derivative_beta_term3, &temp);

		}	//if-else

	}	//n
	return derivative_beta_term3;

}


Matrix Objective::ComputeSecondDerivativeBetaTerm3_() {
	Matrix second_derivative_beta_term3;
	second_derivative_beta_term3.Init(betas.length(), betas.length());
	second_derivative_beta_term3.SetAll(0.0);

	Matrix first_temp;
	first_temp.Init(betas.length(), betas.length());

	Matrix second_temp;
	second_temp.Init(betas.length(), betas.length());

	Matrix second_derivative_beta_temp;
	second_derivative_beta_temp.Init(betas.length(), betas.length());

	for(index_t n=0; n<first_stage_x_.size(); n++){
		  if (second_stage_y_[n]<0) { 
				continue;
			} else {
				la::Scale( (1/(postponed_probability_[n]), &first_temp);
				la::MulTransBOverwrite(sum_first_derivative_conditional_postpond_prob_[n], 
															 sum_first_derivative_conditional_postpond_prob_[n], &second_temp);
				la::Scale( 1/pow((1-postponed_probability_[n]), 2), &second_temp);

				la::SubOverwrite(second_temp, first_temp, &second_derivative_beta_temp);

				//check
				la::AddTo(second_derivative_beta_temp, &second_derivative_beta_term3);

							
			}	//else

	}	//n
	
	return second_derivative_beta_term3;
}



double Object::ComputeDerivativePTerm1_() {
	double derivative_p_term1=0;

	return derivative_p_term1;
}



double Objective::ComputeSecondDerivativePTerm1_() {
	double second_derivative_p_term1=0;

	return second_derivative_p_term1;
}



double Object::ComputeDerivativeQTerm1_() {
	double derivative_q_term1=0;

	return derivative_q_term1;
}



double Objective::ComputeSecondDerivativeQTerm1_() {
	double second_derivative_q_term1=0;

	return second_derivative_q_term1;
}

void Objective::ComputeSumDerivativeBetaFunction_(Vector &betas, double p, double q) {
	double alpha_temp=0;
	double t_temp=0;

	num_of_alphas_=10;
	num_of_t_beta_fn_=10;
	alpha_weight_=1/num_of_alphas;
	t_weight_=1/num_of_t_beta_fn;

	//check - don't need to initilize memeber variable here
	sum_first_derivative_p_beta_fn_.SetZero();
	sum_second_derivative_p_beta_fn_.SetZero();
	sum_first_derivative_q_beta_fn_.SetZero();
	sum_second_derivative_q_beta_fn_.SetZero();
	sum_second_derivative_p_q_beta_fn_.SetZero();


	double beta_fn_temp1=0;
	double beta_fn_temp2=0;
	double beta_fn_temp3=0;
	double beta_fn_temp4=0;
	double beta_fn_temp5=0;
	double beta_fn_temp6=0;

	beta_fn_temp1=1/denumerator_beta_function_;

	for(index_t m=0; m<num_of_t_beta_fn-1; m++){
		t_temp=(m+1)*(t_weight_);

		beta_fn_temp2+=pow(t_temp, p-1)*pow(1-t_temp, q-1)*log(t_temp);
		beta_fn_temp3+=pow(t_temp, p-1)*pow(1-t_temp, q-1)*pow(log(t_temp), 2);
		beta_fn_temp4+=pow(t_temp, p-1)*pow(1-t_temp, q-1)*log(1-t_temp);
		beta_fn_temp5+=pow(t_temp, p-1)*pow(1-t_temp, q-1)*pow(log(1-t_temp), 2);
		beta_fn_temp6+=pow(t_temp, p-1)*pow(1-t_temp, q-1)*log(1-t_temp)*log(t_temp);


	}		//m
	beta_fn_temp2*=(t_weight_/pow(denumerator_beta_function_, 2));
	beta_fn_temp3*=(t_weight_/pow(denumerator_beta_function_, 2));
	beta_fn_temp4*=(t_weight_/pow(denumerator_beta_function_, 2));
	beta_fn_temp5*=(t_weight_/pow(denumerator_beta_function_, 2));

	for(index_t n=0; n<first_stage_x_.size(); n++){
		for(index_t l=0; l<num_of_alphas-1; l++){
			alpha_temp=(l+1)*(alpha_weight_);

			//Calculate x^2_{ni}(alpha_l)
			for(index t i=1; i<first_stage_x_[n].n_cols(); i++){
				int count=0;
				for(index_t j=ind_unk_x_[0]; j<ind_unk_x_[ind_unk_x_.size()]; j++){
					count+=1;
					exponential_temp=alpha_temp*first_stage_x_[n].get(i, j)
													+(alpha_temp)*(1-alpha_temp)*unk_x_past[i].get(count-1,1)
													+(alpha_temp)*pow((1-alpha_temp),2)*unk_x_past[i].get(count-1,2);
					second_stage_x_[n].set(j, i, exponential_temp);
				}	//j
			}	//i

			//Calculate e^(beta*x^2(alpha_l))
			for(index_t i=0; i<second_stage_x_[n].n_cols(); i++) {
				exp_betas_times_x2_[n]+=exp(la::Dot(betas.size(), betas.ptr(),
											 second_stage_x_[n].GetColumnPtr(i) ));
				/*//Calculate second_stage_dot_logit_
				second_stage_dot_logit_[n].set( i, 1, exp(la::Dot(betas.length(), beta.ptr(),
																				 (second_stage_x_[n].GetColumnPtr(i))/
																				 exp_betas_times_x2_[n]) );
				second_stage_ddot_logit_[n].set(i, i, first_stage_dot_logit_[n].get(i,1));
				*/
			}	//i
			conditional_postponed_prob=exp_betas_times_x2_[n]/(exp_betas_times_x1_[n]+exp_betas_times_x2_[n]);

			sum_first_derivative_p_beta_fn_[n]+=conditional_postponed_prob
																					*( pow(alpha_temp, p-1)*pow(1-alpha_temp, q-1)
																					*(log(alpha_temp)*beta_fn_temp1
																					- beta_fn_temp2) );
			sum_second_derivative_p_beta_fn_[n]+=conditional_postponed_prob
																					 *( pow(alpha_temp, p-1)*pow(1-alpha_temp, q-1)
																					 *( pow(log(alpha_temp), 2)*beta_fn_temp1
																					 -2*log(alpha_temp)*beta_fn_temp2
																					 +beta_fn_temp3));
			sum_first_derivative_q_beta_fn_[n]+=conditional_postponed_prob
																					*( pow(alpha_temp, p-1)*pow(1-alpha_temp, q-1)
																					*(log(1-alpha_temp)*beta_fn_temp1
																					- beta_fn_temp4) );
			sum_second_derivative_q_beta_fn_[n]+=conditional_postponed_prob
																					 *( pow(alpha_temp, p-1)*pow(1-alpha_temp, q-1)
																					 *( pow(log(1-alpha_temp), 2)*beta_fn_temp1
																					 -2*log(1-alpha_temp)*beta_fn_temp4
																					 +beta_fn_temp5));
			sum_second_derivative_p_q_beta_fn_=0;

		}	//l
		sum_first_derivative_p_beta_fn_[n]*=alpha_weight_;
		sum_second_derivative_p_beta_fn_[n]*=alpha_weight_;
		sum_first_derivative_q_beta_fn_[n]*=alpha_weight_;
		sum_second_derivative_q_beta_fn_[n]*=alpha_weight_;


	}	//n





	



	

}


