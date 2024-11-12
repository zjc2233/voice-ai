import axios, { AxiosInstance, AxiosRequestConfig, AxiosResponse } from 'axios'

// 定义请求配置类型
interface RequestConfig extends AxiosRequestConfig {
  ssr?: boolean // 是否支持 SSR
  customHeaders?: Record<string, string> // 自定义请求头
}

// 创建 Axios 实例
const instance: AxiosInstance = axios.create({
  baseURL: 'http://localhost:5173', // 根据环境变量设置 baseURL
  timeout: 100000 // 请求超时时间
})

// 请求拦截器
instance.interceptors.request.use(
  (config: RequestConfig) => {
    // 添加自定义请求头
    if (config.customHeaders) {
      Object.keys(config.customHeaders).forEach((key) => {
        config.headers[key] = config.customHeaders[key]
      })
    }

    // 如果是 SSR 请求，可以在这里处理 SSR 相关的逻辑
    if (config.ssr) {
      // SSR 相关处理
    }

    return config
  },
  (error) => {
    return Promise.reject(error)
  }
)

// 响应拦截器
instance.interceptors.response.use(
  (response: AxiosResponse) => {
    // 处理响应数据
    return response.data
  },
  (error) => {
    // 处理响应错误
    return Promise.reject(error)
  }
)

// 封装请求方法
const request = <T>(config: RequestConfig): Promise<T> => {
  return instance.request<T>(config).then(response => response.data);
}

export default request
